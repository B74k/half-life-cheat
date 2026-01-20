#include "include/sdk.h"

void bhop_tick(usercmd_t* cmd) {
    if (!(cmd->buttons & IN_JUMP)) return;
    if (!g_player_move || !(g_player_move->flags & FL_ONGROUND))
        cmd->buttons &= ~IN_JUMP;
}
