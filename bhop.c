#include "include/sdk.h"

void bhop_tick(usercmd_t* cmd) {
    if (!(cmd->buttons & IN_JUMP)) return;
    if (!g_pPlayerMove || !(g_pPlayerMove->flags & FL_ONGROUND))
        cmd->buttons &= ~IN_JUMP;
}
