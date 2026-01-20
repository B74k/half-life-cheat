/*
 * HL25 (25th Anniversary) specific patterns and entity access
 */

#ifndef HL25_H
#define HL25_H

#include "sdk.h"
#include "pattern.h"
#include <string.h>

#define HL25_ENT_RENDER_ORIGIN  0xB48
#define HL25_ENT_MODEL_OFFSET   0xB94
#define HL25_ENT_FRAMERATE      0x310

#define CLDLL_OFFSET_CL_CREATEMOVE  0x38
#define CLDLL_OFFSET_HUD_REDRAW     0x0C
#define CLDLL_OFFSET_CLIENTMOVE     0x18

typedef struct {
    int* p_entity_count;
    void* p_client_state;
    void** p_client_funcs;
    int initialized;
} hl25_addresses_t;

extern hl25_addresses_t g_hl25_addresses;

typedef struct { char name[64]; } model_t;

static inline int hl25_init_addresses(void) {
    module_info_t hw;
    void* m;
    if (g_hl25_addresses.initialized) return 1;
    if (!memory_get_module("hw.dll", &hw) && !memory_get_module("sw.dll", &hw)) return 0;

    m = memory_find_pattern(&hw, "55 8B EC A1 ?? ?? ?? ?? 56 8B 75 08 3B F0 7C");
    if (m) memcpy(&g_hl25_addresses.p_entity_count, (char*)m + 4, 4);

    m = memory_find_pattern(&hw, "B8 ?? ?? ?? ?? C3");
    if (m) memcpy(&g_hl25_addresses.p_client_state, (char*)m + 1, 4);

    m = memory_find_pattern(&hw, "68 AC 00 00 00 6A 00 68");
    if (m) {
        void* tbl; memcpy(&tbl, (char*)m + 8, 4);
        if ((size_t)tbl > (size_t)hw.base && (size_t)tbl < (size_t)hw.base + hw.size + 0x2000000)
            g_hl25_addresses.p_client_funcs = (void**)tbl;
    }

    g_hl25_addresses.initialized = (g_hl25_addresses.p_client_state && g_hl25_addresses.p_entity_count);
    return g_hl25_addresses.initialized;
}

static inline int hl25_get_entity_count(void) {
    return g_hl25_addresses.p_entity_count ? *g_hl25_addresses.p_entity_count : 0;
}

static inline void hl25_entity_get_origin(cl_entity_t* ent, vec3_t out) {
    if (!ent) { VectorClear(out); return; }
    float* o = (float*)((char*)ent + HL25_ENT_RENDER_ORIGIN);
    VectorCopy(o, out);
}

static inline void* hl25_get_model_ptr(cl_entity_t* ent) {
    return ent ? *(void**)((char*)ent + HL25_ENT_MODEL_OFFSET) : NULL;
}

static inline const char* hl25_get_model_name(cl_entity_t* ent) {
    void* m = hl25_get_model_ptr(ent);
    return m ? ((model_t*)m)->name : NULL;
}

static inline int hl25_entity_is_alive(cl_entity_t* ent) {
    return ent && *(float*)((char*)ent + HL25_ENT_FRAMERATE) != 0.0f;
}

static inline cl_entity_t* hl25_get_entity(int index) {
    return (g_engine && g_engine->GetEntityByIndex) ? g_engine->GetEntityByIndex(index) : NULL;
}

#endif
