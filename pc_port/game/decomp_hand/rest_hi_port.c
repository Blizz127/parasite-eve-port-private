/*
 * Hand adapters — remaining >= 0x80060000 leaves (texture upload helpers,
 * program select, reverb depth).  Bodies follow the matched leaves
 * src/func_XXXXXXXX.c; pointers are guest addresses.
 */
#include "pe_guest_decomp.h"
#include "pe_sdk.h"
#include "hand_hi_protos.h"

extern void func_80085DC4(int value);                                   /* generated TU */
extern void func_8008F0D0(pe_addr_t pe_a0, pe_addr_t pe_a1, int a2);  /* generated TU */

/* src/func_80085D84.c: exchange D_8009B438 with a0; on change call
 * func_80085DC4 — declared without arguments, the retail call keeps a0 in
 * $a0 (it is the value just stored) — and return the old value.
 * pe_dis.sh 0x80085D84 0x3C: `jal 0x80085dc4` at 0x80085DA4 with $a0
 * untouched since entry (its delay slot stores a0 to D_8009B438). */
int func_80085D84(int a0)
{
    int old = (int)PE_LoadU32(0x8009B438u);

    if (a0 != old) {
        PE_StoreU32(0x8009B438u, (uint32_t)a0);
        func_80085DC4(a0);
    }
    return old;
}

/* src/func_8008F178.c: program a1 without rescaling — +0x5A = a1, then
 * func_8008F0D0(a0, D_800B2900 + (a1 << 6), its first word). */
void func_8008F178(pe_addr_t a0, int a1)
{
    pe_addr_t p = 0x800B2900u + (uint32_t)(a1 << 6);

    PE_StoreU16(a0 + 0x5Au, (uint16_t)a1);
    func_8008F0D0(a0, p, (int)PE_LoadU32(p));
}

/* src/func_80074774.c: LoadImage a texture page at (a3, a4), a6 lines,
 * width a5 / 4, a5 / 2 or a5 halfwords for 4/8/16-bit mode a1 (signed int
 * division); return its texpage id func_80077A64(a1, a2, a3, a4).  For a
 * mode outside 0..2 retail leaves the width on the stack unset; the host
 * RECT starts at 0 there. */
unsigned short func_80074774(int a0, int a1, int a2, int a3, int a4, int a5, int a6)
{
    RECT r = {0, 0, 0, 0};

    r.x = (int16_t)a3;
    r.h = (int16_t)a6;
    r.y = (int16_t)a4;
    switch (a1) {
    case 0: r.w = (int16_t)(a5 / 4); break;
    case 1: r.w = (int16_t)(a5 / 2); break;
    case 2: r.w = (int16_t)a5; break;
    }
    (void)func_8007506C(&r, (pe_addr_t)a0);
    return (unsigned short)func_80077A64((uint32_t)a1, (uint32_t)a2, (uint32_t)a3, (uint32_t)a4);
}

/* src/func_800CED3C.c: upload the 64x256 effect texture at (0x380, 0x100)
 * from D_800B0E18 (a0 == 0) or D_800B0E1C, and record a0 in D_800F34E4. */
/* func_800CED3C: ported from the matching decomp -- generated TU pc_port/game/decomp/func_800CED3C_port.c (src/func_800CED3C.c); hand port retired (port3 switch-over N). */
