#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include "include/sdk.h"
#include "include/pattern.h"
#include "include/hl25.h"

void bhop_tick(usercmd_t*);
void esp_draw(void);

cl_enginefunc_t* g_engine;
cldll_func_t* g_client;
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
    module_info_t hw, cl;
    DWORD old;
    static cldll_func_t funcs;
    void* m;

    if (!memory_get_module("hw.dll", &hw) && !memory_get_module("sw.dll", &hw)) return;
    if (!memory_get_module("client.dll", &cl)) return;

    void (*F)(cldll_func_t*) = (void(*)(cldll_func_t*))GetProcAddress((HMODULE)cl.base, "F");
    if (!F) return;
    F(&funcs);
    g_client = &funcs;

    m = memory_find_pattern(&hw, "6A 07 68 ?? ?? ?? ?? FF 15");
    if (m) { void* e = *(void**)((char*)m + 3); if ((size_t)e > 0x10000000) g_engine = e; }

    if (g_engine) { g_screen.iSize = sizeof(g_screen); g_engine->pfnGetScreenInfo(&g_screen); }

    orig_create_move = funcs.pCL_CreateMoveFunc;
    orig_redraw = funcs.pHudRedrawFunc;
    orig_player_move = funcs.pClientMove;

    if (g_hl25_addresses.p_client_funcs) {
        void** t = g_hl25_addresses.p_client_funcs;
        if (VirtualProtect(t, 0xAC, PAGE_EXECUTE_READWRITE, &old)) {
            t[CLDLL_OFFSET_CL_CREATEMOVE/4] = hook_createmove;
            t[CLDLL_OFFSET_HUD_REDRAW/4] = hook_redraw;
            t[CLDLL_OFFSET_CLIENTMOVE/4] = hook_playermove;
            VirtualProtect(t, 0xAC, old, &old);
            return;
        }
    }
    funcs.pCL_CreateMoveFunc = hook_createmove;
    funcs.pHudRedrawFunc = hook_redraw;
    funcs.pClientMove = hook_playermove;
}

static void restore_hooks(void) {
    DWORD old;
    if (g_hl25_addresses.p_client_funcs) {
        void** t = g_hl25_addresses.p_client_funcs;
        if (VirtualProtect(t, 0xAC, PAGE_EXECUTE_READWRITE, &old)) {
            if (orig_create_move) t[CLDLL_OFFSET_CL_CREATEMOVE/4] = orig_create_move;
            if (orig_redraw) t[CLDLL_OFFSET_HUD_REDRAW/4] = orig_redraw;
            if (orig_player_move) t[CLDLL_OFFSET_CLIENTMOVE/4] = orig_player_move;
            VirtualProtect(t, 0xAC, old, &old);
        }
    }
    if (g_client) {
        if (orig_create_move) g_client->pCL_CreateMoveFunc = orig_create_move;
        if (orig_redraw) g_client->pHudRedrawFunc = orig_redraw;
        if (orig_player_move) g_client->pClientMove = orig_player_move;
    }
}

static DWORD WINAPI init_thread(LPVOID p) {
    (void)p; Sleep(2000);
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
