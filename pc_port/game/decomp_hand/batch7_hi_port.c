/*
 * Hand adapters — batch 7 (>= 0x80060000).  Bodies follow the matched leaves
 * src/func_XXXXXXXX.c; pointers are guest addresses.  Callees with no
 * pc_port implementation stay loud boundaries (PE_D_COMP_BOUNDARYn).
 */
#include "pe_guest_decomp.h"
#include "pe_sdk.h"
#include "hand_hi_protos.h"

extern int func_8007FC88(pe_addr_t pe_result);
extern void func_80080998(pe_addr_t dest, pe_addr_t source);
extern int func_800C6CE0(pe_addr_t slot);

/* src/func_8008E840.c: SPU pitch from the 16-word-per-row table
 * D_800B2910 + a0 * 0x40: n = a1 & 0xFF, entry n % 12, octave q = n / 12;
 * optional fine tune v += (v * a2) >> 7; shift left by q - 6 for q >= 7,
 * right by 6 - q for q < 6; return the low 16 bits. */
unsigned int func_8008E840(int a0, unsigned int a1, int a2)
{
    unsigned int n = a1 & 0xFF;
    unsigned int q = n / 12;
    unsigned int m = n % 12;
    pe_addr_t base = (pe_addr_t)(a0 * 0x40) + 0x800B2910u;
    unsigned int v = PE_LoadU32(base + m * 4u);

    if (a2 != 0)
        v = v + ((v * (unsigned int)a2) >> 7);
    if (q >= 7)
        v <<= q - 6;
    else if (q < 6)
        v >>= 6 - q;
    return v & 0xFFFF;
}

/* src/func_800CD50C.c: draw-state setup (page 3, 0x10 colours, 0x20x0x20,
 * blend 3), copy a2's +8/+A/+C shorts to D_800F348C/90/94 (sign-extended
 * words) and +4 (u16) to D_800E27EA, then func_800C42A4(&D_800E27EA - 5
 * halfwords, &D_800F348C - 5 words, 1). */
void func_800CD50C(int a0, int a1, pe_addr_t a2)
{
    (void)a0; (void)a1;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(3);
    PE_StoreU32(0x800F348Cu, (uint32_t)(int32_t)(int16_t)PE_LoadU16(a2 + 8u));
    PE_StoreU32(0x800F3490u, (uint32_t)(int32_t)(int16_t)PE_LoadU16(a2 + 0xAu));
    PE_StoreU32(0x800F3494u, (uint32_t)(int32_t)(int16_t)PE_LoadU16(a2 + 0xCu));
    PE_StoreU16(0x800E27EAu, PE_LoadU16(a2 + 4u));
    func_800C42A4(0x800E27EAu - 10u, 0x800F348Cu - 20u, 1);
}

/* src/func_800CECAC.c: two texpage words each for (0x380,0x100) at
 * D_800E2850/54 and (0x340,0x100) at D_800E2852/56 (i = 0, 1; abr 0). */
void func_800CECAC(void)
{
    int i;

    for (i = 0; i < 2; i++)
        PE_StoreU16(0x800E2850u + (uint32_t)i * 4u,
                    (uint16_t)func_80077A64((uint32_t)i, 0u, 0x380u, 0x100u));
    for (i = 0; i < 2; i++)
        PE_StoreU16(0x800E2852u + (uint32_t)i * 4u,
                    (uint16_t)func_80077A64((uint32_t)i, 0u, 0x340u, 0x100u));
}

/* src/func_8007F608.c: func_8007FC88(0); if D_800A3530 == 1 (priority) or
 * else D_800A3520 == 1, clear that slot's word, copy its bytes from +5 into
 * a0 (func_80080998) and return its +4 byte; otherwise 0. */
int func_8007F608(int a0)
{
    pe_addr_t p;

    (void)func_8007FC88(0u);
    if (PE_LoadU32(0x800A3530u) == 1u)
        p = 0x800A3530u;
    else if (PE_LoadU32(0x800A3520u) == 1u)
        p = 0x800A3520u;
    else
        return 0;
    PE_StoreU32(p, 0u);
    func_80080998((pe_addr_t)a0, p + 5u);
    return PE_LoadU8(p + 4u);
}

/* src/func_800CDF4C.c: fade f1 (s8 +1) by 0x10, grow f3 (s8 +3) by 0xC,
 * integrate eight points (x/y/z arrays at +0x10/+0x20/+0x30 by
 * dx/dy/dz at +0x40/+0x50/+0x60); a1[1] = 2 once f1 < 0x10. */
void func_800CDF4C(int a0, pe_addr_t a1, pe_addr_t a2)
{
    unsigned short i;

    (void)a0;
    PE_StoreU8(a2 + 1u, (uint8_t)((signed char)PE_LoadU8(a2 + 1u) - 0x10));
    PE_StoreU8(a2 + 3u, (uint8_t)((signed char)PE_LoadU8(a2 + 3u) + 0xC));
    for (i = 0; i < 8; i++) {
        uint32_t k = i * 2u;

        PE_StoreU16(a2 + 0x10u + k, (uint16_t)(PE_LoadU16(a2 + 0x10u + k) + PE_LoadU16(a2 + 0x40u + k)));
        PE_StoreU16(a2 + 0x20u + k, (uint16_t)(PE_LoadU16(a2 + 0x20u + k) + PE_LoadU16(a2 + 0x50u + k)));
        PE_StoreU16(a2 + 0x30u + k, (uint16_t)(PE_LoadU16(a2 + 0x30u + k) + PE_LoadU16(a2 + 0x60u + k)));
    }
    if ((signed char)PE_LoadU8(a2 + 1u) < 0x10)
        PE_StoreU8(a1 + 1u, 2u);
}

/* src/func_800C66C8.c / 800C6764 / 800C6800: when the slot state
 * (func_800C6CE0, evaluated twice as in retail) is 3 or 4, call
 * func_8006DCE4(a1, a0->+8->+0->+8, three shorts of a2); else 0.  The
 * three variants differ only in the short offsets. */
static int slot_probe(pe_addr_t a0, pe_addr_t a1, pe_addr_t a2,
                      uint32_t ox, uint32_t oy, uint32_t oz)
{
    int r = 0;

    if (func_800C6CE0(a0) == 3 || func_800C6CE0(a0) == 4) {
        pe_addr_t w = PE_LoadU32(PE_LoadU32(a0 + 8u));

        r = func_8006DCE4(a1, PE_LoadU32(w + 8u),
                          (int16_t)PE_LoadU16(a2 + ox),
                          (int16_t)PE_LoadU16(a2 + oy),
                          (int16_t)PE_LoadU16(a2 + oz));
    }
    return r;
}
int func_800C66C8(pe_addr_t a0, pe_addr_t a1, pe_addr_t a2)
{
    return slot_probe(a0, a1, a2, 0x14u, 0x18u, 0x1Cu);
}
int func_800C6764(pe_addr_t a0, pe_addr_t a1, pe_addr_t a2)
{
    return slot_probe(a0, a1, a2, 0x0u, 0x4u, 0x8u);
}
int func_800C6800(pe_addr_t a0, pe_addr_t a1, pe_addr_t a2)
{
    return slot_probe(a0, a1, a2, 0x0u, 0x2u, 0x4u);
}

/* src/func_80074218.c: interrupt-state teardown at D_800945E4 (inactive ->
 * NULL).  EnterCriticalSection; save *D_80095674 (u16) to +0x32 and
 * *D_80095678 (word) to +0x34; mask = 0; *D_80095670 = *D_80095674 (now 0);
 * *D_80095678 &= 0x77777777; func_80074394() (no pc_port implementation:
 * boundary); active = 0; return the state block. */
pe_addr_t func_80074218(void)
{
    const pe_addr_t p = 0x800945E4u;
    pe_addr_t q, r, u;

    if (PE_LoadU16(p) == 0u)
        return 0u;
    (void)func_80072714();
    q = PE_LoadU32(0x80095674u);
    r = PE_LoadU32(0x80095678u);
    PE_StoreU16(p + 0x32u, PE_LoadU16(q));
    u = PE_LoadU32(0x80095670u);
    PE_StoreU32(p + 0x34u, PE_LoadU32(r));
    PE_StoreU16(q, 0u);
    PE_StoreU16(u, PE_LoadU16(q));
    r = PE_LoadU32(0x80095678u);
    PE_StoreU32(r, PE_LoadU32(r) & 0x77777777u);
    PE_D_COMP_BOUNDARY0("func_80074394", 0x80074394u);
    PE_StoreU16(p, 0u);
    return p;
}

extern int32_t func_80077CF4(int32_t angle);
extern int32_t func_80077DC4(int32_t angle);

/* src/func_800C4E50.c: fill two rings of r->fC SVECs at r->v (+0): the
 * first scaled by r->f10 (+0x10), the second by r->fE (+0xE), from the
 * sin/cos table leaves over step = 0x1000 / r->fC; vz = 0.  Retail guards
 * the divide with `break 7` (0x800C4E90) when fC == 0: that trap is a loud
 * boundary here rather than a host SIGFPE. */
void func_800C4E50(pe_addr_t r)
{
    pe_addr_t v = PE_LoadU32(r);
    unsigned int n = PE_LoadU16(r + 0xCu);
    int a = 0;
    int step;
    unsigned int i;

    if (n == 0u) {
        PE_D_COMP_BOUNDARY0("func_800C4E50_divide_trap", 0x800C4E90u);
        return;
    }
    step = 0x1000 / (int)n;
    for (i = 0; i < PE_LoadU16(r + 0xCu); i++) {
        PE_StoreU16(v + 0u, (uint16_t)((func_80077CF4(a) * (int)PE_LoadU16(r + 0x10u)) >> 12));
        PE_StoreU16(v + 2u, (uint16_t)((func_80077DC4(a) * (int)PE_LoadU16(r + 0x10u)) >> 12));
        PE_StoreU16(v + 4u, 0u);
        a += step;
        v += 8u;
    }
    a = 0;
    for (i = 0; i < PE_LoadU16(r + 0xCu); i++) {
        PE_StoreU16(v + 0u, (uint16_t)((func_80077CF4(a) * (int)PE_LoadU16(r + 0xEu)) >> 12));
        PE_StoreU16(v + 2u, (uint16_t)((func_80077DC4(a) * (int)PE_LoadU16(r + 0xEu)) >> 12));
        PE_StoreU16(v + 4u, 0u);
        a += step;
        v += 8u;
    }
}
