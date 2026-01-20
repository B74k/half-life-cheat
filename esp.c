#include "include/sdk.h"
#include "include/hl25.h"
#include <string.h>

static int to_screen(vec3_t world, vec2_t out) {
    vec2_t n;
    if (!g_engine || !g_engine->pTriAPI) return 0;
    if (g_engine->pTriAPI->WorldToScreen(world, n)) return 0;
    if (n[0] < -1 || n[0] > 1 || n[1] < -1 || n[1] > 1) return 0;
    if (g_screen.iWidth <= 0 || g_screen.iHeight <= 0) return 0;
    out[0] = (n[0] + 1) * g_screen.iWidth / 2;
    out[1] = (1 - n[1]) * g_screen.iHeight / 2;
    return 1;
}

static const char* npc_name(const char* m) {
    // Half-Life
    if (strstr(m, "scientist")) return "Scientist";
    if (strstr(m, "barney")) return "Barney";
    if (strstr(m, "hgrunt")) return "H.Grunt";
    if (strstr(m, "agrunt")) return "A.Grunt";
    if (strstr(m, "islave")) return "Vortigaunt";
    if (strstr(m, "zombie")) return "Zombie";
    if (strstr(m, "headcrab")) return "Headcrab";
    if (strstr(m, "bullsquid")) return "Bullsquid";
    if (strstr(m, "houndeye")) return "Houndeye";
    if (strstr(m, "barnacle")) return "Barnacle";
    if (strstr(m, "garg")) return "Gargantua";
    if (strstr(m, "controller")) return "Controller";
    if (strstr(m, "turret")) return "Turret";
    if (strstr(m, "sentry")) return "Sentry";
    if (strstr(m, "gman")) return "G-Man";
    if (strstr(m, "apache")) return "Apache";
    if (strstr(m, "tentacle")) return "Tentacle";
    if (strstr(m, "nihilanth")) return "Nihilanth";
    if (strstr(m, "ichthyosaur")) return "Ichthyosaur";
    if (strstr(m, "leech")) return "Leech";
    // Opposing Force
    if (strstr(m, "pitdrone")) return "Pit Drone";
    if (strstr(m, "shocktrooper") || strstr(m, "strooper")) return "Shock Trooper";
    if (strstr(m, "voltigore")) return "Voltigore";
    if (strstr(m, "gonome")) return "Gonome";
    if (strstr(m, "massn")) return "Black Ops";
    if (strstr(m, "fassn")) return "Assassin";
    if (strstr(m, "rgrunt")) return "Robot Grunt";
    if (strstr(m, "pitworm")) return "Pit Worm";
    if (strstr(m, "geneworm")) return "Gene Worm";
    if (strstr(m, "shockroach")) return "Shock Roach";
    if (strstr(m, "babygarg")) return "Baby Garg";
    if (strstr(m, "penguin")) return "Penguin";
    // Blue Shift / Shared
    if (strstr(m, "otis")) return "Otis";
    if (strstr(m, "rosenberg")) return "Rosenberg";
    if (strstr(m, "chumtoad")) return "Chumtoad";
    return NULL;
}

void esp_draw(void) {
    if (!g_engine || !g_engine->GetLocalPlayer()) return;

    int count = hl25_get_entity_count();
    if (count <= 0 || count > 512) count = 512;

    for (int i = 2; i < count; i++) {
        cl_entity_t* ent = hl25_get_entity(i);
        if (!ent || !hl25_get_model_ptr(ent)) continue;

        const char* model = hl25_get_model_name(ent);
        if (!model || !strstr(model, "models/")) continue;
        if (strstr(model, "/w_") || strstr(model, "/p_") || strstr(model, "/v_")) continue;
        if (strstr(model, "gibs") || strstr(model, "shell") || strstr(model, "player")) continue;

        const char* name = npc_name(model);
        if (!name || !hl25_entity_is_alive(ent)) continue;

        vec3_t pos; vec2_t scr;
        hl25_entity_get_origin(ent, pos);
        if (pos[0] == 0 && pos[1] == 0 && pos[2] == 0) continue;

        if (!to_screen(pos, scr)) continue;

        int tw, th;
        g_engine->pfnDrawSetTextColor(0.7f, 1.0f, 1.0f);
        g_engine->pfnDrawConsoleStringLen((char*)name, &tw, &th);
        g_engine->pfnDrawConsoleString((int)scr[0] - tw/2, (int)scr[1], (char*)name);
    }
}
