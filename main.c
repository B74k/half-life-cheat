#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include "include/sdk.h"
#include "include/pattern.h"
#include "include/hl25.h"

void bhop_tick(usercmd_t*);
void esp_draw(void);

cl_enginefunc_t* g_engine;
playermove_t* g_player_move;
SCREENINFO g_screen;
hl25_addresses_t g_hl25_addresses;

static CL_CreateMove_t orig_create_move;
static HUD_Redraw_t orig_redraw;
static HUD_PlayerMove_t orig_player_move;

static void __cdecl hook_createmove(float ft, usercmd_t* cmd, int active) {
    if (orig_create_move) orig_create_move(ft, cmd, active);
    if (active) bhop_tick(cmd);
}

static int __cdecl hook_redraw(float time, int intermission) {
    static float last = 0;
    int r = orig_redraw ? orig_redraw(time, intermission) : 0;
    if (time - last > 1.0f) { g_screen.iSize = sizeof(g_screen); if (g_engine) g_engine->pfnGetScreenInfo(&g_screen); last = time; }
    if (!intermission) esp_draw();
    return r;
}

static void __cdecl hook_playermove(playermove_t* pm, int server) {
    g_player_move = pm;
    if (orig_player_move) orig_player_move(pm, server);
}

static void install_hooks(void) {
    module_info_t hw;
    DWORD old;
    void* m;

    if (!memory_get_module("hw.dll", &hw) && !memory_get_module("sw.dll", &hw)) return;
    if (!g_hl25_addresses.p_client_funcs) return;

    m = memory_find_pattern(&hw, "6A 07 68 ?? ?? ?? ?? FF 15");
    if (m) {
        void* e = *(void**)((char*)m + 3);
        if ((size_t)e > 0x10000000) g_engine = e;
    }

    if (g_engine) {
        g_screen.iSize = sizeof(g_screen);
        g_engine->pfnGetScreenInfo(&g_screen);
    }

    void** t = g_hl25_addresses.p_client_funcs;
    orig_create_move = (CL_CreateMove_t)t[CLDLL_OFFSET_CL_CREATEMOVE/4];
    orig_redraw = (HUD_Redraw_t)t[CLDLL_OFFSET_HUD_REDRAW/4];
    orig_player_move = (HUD_PlayerMove_t)t[CLDLL_OFFSET_CLIENTMOVE/4];

    if (VirtualProtect(t, 0xAC, PAGE_EXECUTE_READWRITE, &old)) {
        t[CLDLL_OFFSET_CL_CREATEMOVE/4] = hook_createmove;
        t[CLDLL_OFFSET_HUD_REDRAW/4] = hook_redraw;
        t[CLDLL_OFFSET_CLIENTMOVE/4] = hook_playermove;
        VirtualProtect(t, 0xAC, old, &old);
    }
}

static void restore_hooks(void) {
    DWORD old;
    if (!g_hl25_addresses.p_client_funcs) return;
    void** t = g_hl25_addresses.p_client_funcs;
    if (VirtualProtect(t, 0xAC, PAGE_EXECUTE_READWRITE, &old)) {
        if (orig_create_move) t[CLDLL_OFFSET_CL_CREATEMOVE/4] = orig_create_move;
        if (orig_redraw) t[CLDLL_OFFSET_HUD_REDRAW/4] = orig_redraw;
        if (orig_player_move) t[CLDLL_OFFSET_CLIENTMOVE/4] = orig_player_move;
        VirtualProtect(t, 0xAC, old, &old);
    }
}

static DWORD WINAPI init_thread(LPVOID p) {
    (void)p;
    Sleep(2000);
    hl25_init_addresses();
    install_hooks();
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE inst, DWORD reason, LPVOID r) {
    (void)r;
    if (reason == DLL_PROCESS_ATTACH) { DisableThreadLibraryCalls(inst); CreateThread(0,0,init_thread,0,0,0); }
    else if (reason == DLL_PROCESS_DETACH) restore_hooks();
    return TRUE;
}
