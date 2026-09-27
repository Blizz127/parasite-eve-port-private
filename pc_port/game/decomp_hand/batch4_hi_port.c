/*
 * Hand adapters — batch 4 (>= 0x80060000): sound-command record setters,
 * table getters, overlay sound-parameter fillers, CD callback selectors.
 *
 * Bodies follow the matched leaves src/func_XXXXXXXX.c; pointers are guest
 * addresses.  Callees with no pc_port implementation stay loud boundaries
 * (PE_D_COMP_BOUNDARYn), exactly as a generated TU would spell them.
 */
#include "pe_guest_decomp.h"
#include "pe_spu_dma.h"
#include "hand_hi_protos.h"

extern void func_8008AE94(pe_addr_t pe_a0);

/* src/func_8008B1D0.c: func_8008A400(arg0->+4, arg0->+8) (absent_hi2_port.c). */
void func_8008B1D0(pe_addr_t arg0)
{
    func_8008A400(PE_LoadU32(arg0 + 4u), PE_LoadU32(arg0 + 8u));
}

/* src/func_8008227C.c: a0 == 2 -> func_800824C8(func_80081E70). */
void func_8008227C(unsigned char a0)
{
    if (a0 == 2)
        (void)func_800824C8((int)0x80081E70u);
}

/* src/func_800878C0.c: halfword at 0x1F801C0A + (voice << 4):
 * (*reg & 0xFFC0) | ((mode >> 2) << 5) | low. */
void func_800878C0(int voice, unsigned int low, unsigned int mode)
{
    uint32_t off = 0xAu + (uint32_t)(voice << 4);

    PE_SpuRegister_StoreU16(off,
        (unsigned short)((PE_SpuRegister_LoadU16(off) & 0xFFC0u) |
                         (((mode >> 2) << 5) | low)));
}

/* src/func_800755BC.c / 80075AE8.c: func_80071A34(a0, template, 0x5C/0x14)
 * then return a0.  func_80071A34 is the BIOS A(2Ah) trampoline (`li t2,0xA0;
 * jr t2; li t1,0x2A`) with no matched C: loud boundary. */
pe_addr_t func_800755BC(pe_addr_t a0)
{
    func_80071A34(a0, 0x8009575Cu, 0x5C);
    return a0;
}
pe_addr_t func_80075AE8(pe_addr_t a0)
{
    func_80071A34(a0, 0x800957B8u, 0x14);
    return a0;
}

/* src/func_8007A400.c: i = a0 & 0xFF; i >= 0x1C -> &D_800119CC, else
 * D_8009AFDC[i]. */
unsigned int func_8007A400(unsigned int a0)
{
    unsigned int i = a0 & 0xFF;

    if (i >= 0x1C)
        return 0x800119CCu;
    return PE_LoadU32(0x8009AFDCu + i * 4u);
}

/* src/func_8007A434.c: i = a0 & 0xFF; i >= 7 -> &D_800119CC, else
 * D_8009B05C[i]. */
unsigned int func_8007A434(unsigned int a0)
{
    unsigned int i = a0 & 0xFF;

    if (i >= 7)
        return 0x800119CCu;
    return PE_LoadU32(0x8009B05Cu + i * 4u);
}

/* src/func_8007C444.c: zero word 0 of a1 consecutive 32-byte records at
 * D_800C0DC8 starting with record a0. */
void func_8007C444(int a0, unsigned int a1)
{
    unsigned int i = 0;

    if (a1 == 0)
        return;
    do {
        int off = (int)i + a0;
        i++;
        PE_StoreU32(PE_LoadU32(0x800C0DC8u) + (uint32_t)(off << 5), 0u);
    } while (i < a1);
}

/* src/func_8008594C.c: u = a0 & 0xFFFF; u >= 3 -> 0; else clear the
 * halfword at D_8009B7D0 + u * 16 and return 1. */
int func_8008594C(int a0)
{
    int u = a0 & 0xFFFF;

    if (u >= 3)
        return 0;
    PE_StoreU16(PE_LoadU32(0x8009B7D0u) + (uint32_t)(u * 16), 0u);
    return 1;
}

/* src/func_80083790.c: ((a0->+0xE3 + 1) >> 1) << 2  +
 * ((a0->+0xE9 * 5 + 3) & 0xFFC) + 4  +  a0->+0xEC. */
int func_80083790(pe_addr_t a0)
{
    unsigned int v = ((PE_LoadU8(a0 + 0xE3u) + 1u) >> 1) << 2;
    unsigned int b = PE_LoadU8(a0 + 0xE9u);
    unsigned int off = ((b * 5u + 3u) & 0xFFCu) + 4u;

    return (int)(v + off + PE_LoadU32(a0 + 0xECu));
}

/* src/func_8008B580.c: D_8009D2A2 = 0; D_8009D2B4 = (u16)arg0->+4 << 16;
 * func_8008D7D0(). */
void func_8008B580(int arg0)
{
    unsigned short v = PE_LoadU16((pe_addr_t)arg0 + 4u);

    PE_StoreU16(0x8009D2A2u, 0u);
    PE_StoreU32(0x8009D2B4u, (uint32_t)v << 16);
    func_8008D7D0();
}

/* src/func_800C811C.c (and twins 800CAB88 / 800CD07C with their own
 * globals and constant): a2[4] = G0; a2[5] = D_8009D254->+0x2E;
 * a2[2] = 0x7F; a2[3] = K; a2[6] = G1 (volatile order preserved). */
static void c811c_fill(pe_addr_t a2, pe_addr_t g0, pe_addr_t g1, uint16_t k)
{
    uint16_t c;

    PE_StoreU16(a2 + 8u, PE_LoadU16(g0));
    PE_StoreU16(a2 + 10u, PE_LoadU16(PE_LoadU32(0x8009D254u) + 0x2Eu));
    c = PE_LoadU16(g1);
    PE_StoreU16(a2 + 4u, 0x7Fu);
    PE_StoreU16(a2 + 6u, k);
    PE_StoreU16(a2 + 12u, c);
}
void func_800C811C(int a0, int a1, pe_addr_t a2)
{
    (void)a0; (void)a1;
    c811c_fill(a2, 0x800E2348u, 0x800E234Cu, 0x224u);
}
void func_800CAB88(int a0, int a1, pe_addr_t a2)
{
    (void)a0; (void)a1;
    c811c_fill(a2, 0x800E2360u, 0x800E2364u, 0x224u);
}
void func_800CD07C(int a0, int a1, pe_addr_t a2)
{
    (void)a0; (void)a1;
    c811c_fill(a2, 0x800E27F8u, 0x800E27FCu, 0x60Cu);
}

/* src/func_800CC244.c / 800CC284 / 800CC440: a2[3] = D_800E2290;
 * a2[4] = D_800E2292 (- 0x64 in CC284); a2[2] = K; byte +3 = 0x7F;
 * a2[5] = D_800E2294. */
static void cc244_fill(pe_addr_t a2, uint16_t y_adjust, uint16_t k)
{
    uint16_t c;

    PE_StoreU16(a2 + 6u, PE_LoadU16(0x800E2290u));
    PE_StoreU16(a2 + 8u, (uint16_t)(PE_LoadU16(0x800E2292u) - y_adjust));
    c = PE_LoadU16(0x800E2294u);
    PE_StoreU16(a2 + 4u, k);
    PE_StoreU8(a2 + 3u, 0x7Fu);
    PE_StoreU16(a2 + 10u, c);
}
void func_800CC244(int a0, int a1, pe_addr_t a2)
{
    (void)a0; (void)a1;
    cc244_fill(a2, 0u, 0xBE8u);
}
void func_800CC284(int a0, int a1, pe_addr_t a2)
{
    (void)a0; (void)a1;
    cc244_fill(a2, 0x64u, 0u);
}
void func_800CC440(int a0, int a1, pe_addr_t a2)
{
    (void)a0; (void)a1;
    cc244_fill(a2, 0u, 0x224u);
}

/* src/func_80082400.c: a0 == 2 -> func_80081D74(func_80082444, 0), else
 * D_800B28F8 = 2. */
void func_80082400(unsigned char a0)
{
    if (a0 == 2)
        func_80081D74((int)0x80082444u, 0);   /* absent_r3_port.c; code pointer = retail VMA */
    else
        PE_StoreU32(0x800B28F8u, 2u);
}

/* src/func_8008B040.c: func_8008AE94(a0); D_8009D22C = a0[4] ? a0[4] - 1 : 0. */
void func_8008B040(int a0)
{
    int r = 0;
    int v;

    func_8008AE94((pe_addr_t)a0);
    v = (int)PE_LoadU32((pe_addr_t)a0 + 16u);
    if (v != 0)
        r = v - 1;
    PE_StoreU32(0x8009D22Cu, (uint32_t)r);
}

/* src/func_8008B084.c: save arg0[1]/[2], reset arg0[1..4] to 0x400,
 * 0x1000000, 0x80, 0x7F, then func_8008A92C(arg0, a, b) — boundary. */
void func_8008B084(pe_addr_t arg0)
{
    int a = (int)PE_LoadU32(arg0 + 4u);
    int b = (int)PE_LoadU32(arg0 + 8u);

    PE_StoreU32(arg0 + 4u, 0x400u);
    PE_StoreU32(arg0 + 8u, 0x1000000u);
    PE_StoreU32(arg0 + 12u, 0x80u);
    PE_StoreU32(arg0 + 16u, 0x7Fu);
    PE_D_COMP_BOUNDARY3("func_8008A92C", 0x8008A92Cu, arg0, a, b);
}

/* src/func_8008AB1C.c: two lookups in the D_8009D240 u16 offset table at
 * ((a2 & 0x3FF) << 1) and the next entry; 0xFFFF -> NULL, otherwise
 * D_8009D260 + offset.  Computed host-side, then stored as guest words. */
static void ab1c_lookup(unsigned int a2, pe_addr_t *r0, pe_addr_t *r1)
{
    pe_addr_t table = PE_LoadU32(0x8009D240u);
    unsigned int v;

    a2 = (a2 & 0x3FFu) << 1;
    v = PE_LoadU16(table + a2 * 2u);
    *r0 = (v != 0xFFFFu) ? PE_LoadU32(0x8009D260u) + v : 0u;
    a2 = a2 + 1;
    v = PE_LoadU16(table + a2 * 2u);
    *r1 = (v != 0xFFFFu) ? PE_LoadU32(0x8009D260u) + v : 0u;
}
void func_8008AB1C(pe_addr_t out0, pe_addr_t out1, unsigned int a2)
{
    pe_addr_t r0, r1;

    ab1c_lookup(a2, &r0, &r1);
    PE_StoreU32(out0, r0);
    PE_StoreU32(out1, r1);
}

/* src/func_8008B124.c: func_8008AB1C(&p1, &p2, arg0->+4) into two stack
 * locals, then func_8008A92C(arg0, p1, p2) — boundary.  The locals never
 * escape beyond the lookup, so they are kept host-side (ab1c_lookup). */
void func_8008B124(int arg0)
{
    pe_addr_t p1, p2;

    ab1c_lookup(PE_LoadU32((pe_addr_t)arg0 + 4u), &p1, &p2);
    PE_D_COMP_BOUNDARY3("func_8008A92C", 0x8008A92Cu, arg0, p1, p2);
}

/* src/func_8008B0C8.c: func_8008AB1C(&x, &y, a0[1]) into stack locals
 * (kept host-side as in func_8008B124), a0[2..4] = 0x2000000, 0x80, 0x7F,
 * then func_8008A92C(a0, x, y) — boundary. */
void func_8008B0C8(pe_addr_t a0)
{
    pe_addr_t x, y;

    ab1c_lookup(PE_LoadU32(a0 + 4u), &x, &y);
    PE_StoreU32(a0 + 8u, 0x2000000u);
    PE_StoreU32(a0 + 12u, 0x80u);
    PE_StoreU32(a0 + 16u, 0x7Fu);
    PE_D_COMP_BOUNDARY3("func_8008A92C", 0x8008A92Cu, a0, x, y);
}
