/**
 * @file patch.c
 * @brief Game-specific hooks, memory patches, and custom symbol intercepts.
 */
#include <stdio.h>
#include <string.h>

#include "imports.h"
#include "log.h"
#include "macho.h"
#include "patch.h"

// Game-specific import tables for the port being developed.
// Register any guest binary symbols you wish to intercept or replace here.
const import_entry patch_imports[] = {
    // Example:
    // F(my_custom_function, my_implementation),
};
const int patch_imports_count = sizeof(patch_imports) / sizeof(patch_imports[0]);

// libstdc++ (pool de spinlocks de __gnu_cxx, 0x478364..0x478c30) usa SWP, que el Cortex-A9 de la Vita trata como
// instrucción indefinida. Cada "swp Rt, Rt2, [Rn]" pasa a un B a un stub LDREX/STREX equivalente y atómico.
// Los stubs pisan los flags: en los 26 sitios la instrucción siguiente es un cmp.
static void patch_swp(void) {
    enum { LO = 0x478300, HI = 0x478c40, EXPECTED = 26 };
    int n = 0;
    for (uint32_t vm = LO; vm < HI; vm += 4) {
        uint32_t w = macho_read_text(vm);
        if ((w & 0xFFF00FF0) != 0xE1000090) // swp (palabra, cond AL)
            continue;
        uint32_t rn = (w >> 16) & 15, rt = (w >> 12) & 15, rt2 = w & 15;
        if (rt == rt2 || rt == rn || rt2 == rn || rn == 12 || rt == 12 || rt2 == 12 || rn == 13)
            fatal_error("swp en 0x%08X con registros no soportados (0x%08X)", (unsigned)vm, (unsigned)w);
        uint32_t code[10] = {
            0xE52DC004,                                     // str   r12, [sp, #-4]!
            0xF57FF05B,                                     // dmb   ish
            0xE1900F9F | rn << 16 | 12 << 12,               // 1: ldrex r12, [Rn]
            0xE1800F90 | rn << 16 | rt << 12 | rt2,         // strex Rt, Rt2, [Rn]
            0xE3500000 | rt << 16,                          // cmp   Rt, #0
            0x1AFFFFFB,                                     // bne   1b
            0xF57FF05B,                                     // dmb   ish
            0xE1A0000C | rt << 12,                          // mov   Rt, r12
            0xE49DC004,                                     // ldr   r12, [sp], #4
            0,                                              // b     vm + 4
        };
        uint32_t stub = macho_alloc_code(code, 0); // dirección donde va a quedar el stub
        code[9] = 0xEA000000 | (((vm + 4 + MACHO_SLIDE) - (stub + 9 * 4 + 8)) >> 2 & 0xFFFFFF);
        macho_alloc_code(code, 10);
        uint32_t b = 0xEA000000 | ((stub - (vm + MACHO_SLIDE + 8)) >> 2 & 0xFFFFFF);
        macho_patch_text(vm, &b, 4);
        n++;
    }
    if (n != EXPECTED)
        fatal_error("swp: %d sitios y se esperaban %d (¿binario distinto?)", n, EXPECTED);
    l_info("swp -> ldrex/strex: %d sitios", n);
}

void patch_init(void) {
    patch_swp();
}

void patch_frame(void) {
    // Per-frame logic if required by target engine
}
