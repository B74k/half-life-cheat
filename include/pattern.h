#ifndef PATTERN_H
#define PATTERN_H

#include <windows.h>
#include <psapi.h>

typedef struct { void* base; size_t size; } module_info_t;

static inline int memory_get_module(const char* name, module_info_t* m) {
    HMODULE h = GetModuleHandleA(name);
    if (!h) return 0;
    MODULEINFO mi;
    if (!GetModuleInformation(GetCurrentProcess(), h, &mi, sizeof(mi))) return 0;
    m->base = mi.lpBaseOfDll;
    m->size = mi.SizeOfImage;
    return 1;
}

static inline int hex(char c) {
    return (c >= '0' && c <= '9') ? c - '0' : (c >= 'A' && c <= 'F') ? c - 'A' + 10 : -1;
}

static inline void* memory_find_pattern(module_info_t* m, const char* pat) {
    for (unsigned char *cur = (unsigned char*)m->base, *end = cur + m->size; cur < end; cur++) {
        const char* p = pat;
        unsigned char* c = cur;
        while (*p) {
            while (*p == ' ') p++;
            if (!*p) return cur;
            if (*p == '?') { p += (p[1] == '?') ? 2 : 1; c++; continue; }
            int hi = hex(p[0]), lo = hex(p[1]);
            if (hi < 0 || lo < 0 || *c++ != (hi << 4 | lo)) break;
            p += 2;
        }
        if (!*p) return cur;
    }
    return NULL;
}

#endif
