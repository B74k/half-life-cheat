#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include "include/sdk.h"
#include "include/pattern.h"
#include "include/hl25.h"

void bhop_tick(usercmd_t*);
void esp_draw(void);

cl_enginefunc_t* g_pEngine;
cldll_func_t* g_pClient;
playermove_t* g_pPlayerMove;
SCREENINFO g_screen;
hl25_addresses_t g_hl25_addresses;

static CL_CreateMove_t g_orig_createmove;
static HUD_Redraw_t g_orig_redraw;
static HUD_PlayerMove_t g_orig_playermove;

static void __cdecl hook_createmove(float ft, usercmd_t* cmd, int active) {
    if (g_orig_createmove) g_orig_createmove(ft, cmd, active);
    if (active) bhop_tick(cmd);
}

static int __cdecl hook_redraw(float time, int intermission) {
    static float last = 0;
    int r = g_orig_redraw ? g_orig_redraw(time, intermission) : 0;
    if (time - last > 1.0f) { g_screen.iSize = sizeof(g_screen); if (g_pEngine) g_pEngine->pfnGetScreenInfo(&g_screen); last = time; }
    if (!intermission) esp_draw();
    return r;
}

static void __cdecl hook_playermove(playermove_t* pm, int server) {
    g_pPlayerMove = pm;
    if (g_orig_playermove) g_orig_playermove(pm, server);
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
    g_pClient = &funcs;

    m = memory_find_pattern(&hw, "6A 07 68 ?? ?? ?? ?? FF 15");
    if (m) { void* e = *(void**)((char*)m + 3); if ((size_t)e > 0x10000000) g_pEngine = e; }

    if (g_pEngine) { g_screen.iSize = sizeof(g_screen); g_pEngine->pfnGetScreenInfo(&g_screen); }

    g_orig_createmove = funcs.pCL_CreateMoveFunc;
    g_orig_redraw = funcs.pHudRedrawFunc;
    g_orig_playermove = funcs.pClientMove;

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
            if (g_orig_createmove) t[CLDLL_OFFSET_CL_CREATEMOVE/4] = g_orig_createmove;
            if (g_orig_redraw) t[CLDLL_OFFSET_HUD_REDRAW/4] = g_orig_redraw;
            if (g_orig_playermove) t[CLDLL_OFFSET_CLIENTMOVE/4] = g_orig_playermove;
            VirtualProtect(t, 0xAC, old, &old);
        }
    }
    if (g_pClient) {
        if (g_orig_createmove) g_pClient->pCL_CreateMoveFunc = g_orig_createmove;
        if (g_orig_redraw) g_pClient->pHudRedrawFunc = g_orig_redraw;
        if (g_orig_playermove) g_pClient->pClientMove = g_orig_playermove;
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
