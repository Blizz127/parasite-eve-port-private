/*
 * Hand adapters — batch 5 (>= 0x80060000): sound-record flag sweeps, table
 * cursors, node-list search, record resets.  Bodies follow the matched
 * leaves src/func_XXXXXXXX.c; pointers are guest addresses.
 */
#include "pe_guest_decomp.h"
/* Generated TU pc_port/game/decomp/func_8008A92C_port.c (src/func_8008A92C.c);
 * native since its func_8008A750 callee became a hand adapter (2026-10-07). */
void func_8008A92C(pe_addr_t pe_a0, int a1, int a2);
#include "hand_hi_protos.h"

/* src/func_8008CB08.c: *a0 = D_800B8628 + D_8009D2F4 * 0x24; D_8009D2F4++. */
void func_8008CB08(pe_addr_t a0)
{
    int n = (int)PE_LoadU32(0x8009D2F4u);

    PE_StoreU32(a0, 0x800B8628u);
    PE_StoreU32(a0, PE_LoadU32(a0) + (uint32_t)(n * 0x24));
    PE_StoreU32(0x8009D2F4u, (uint32_t)(n + 1));
}

/* Shared shape of src/func_8008ABF0.c / 8008AB9C.c: for every set bit of m
 * (starting at `bit`), OR 3 into the +0xF4 word of the matching 0x11C-byte
 * record, clearing the bit until m is empty. */
static void mark_records(unsigned int m, unsigned int bit, pe_addr_t p)
{
    if (m == 0)
        return;
    do {
        if (m & bit) {
            PE_StoreU32(p, PE_LoadU32(p) | 3u);
            m ^= bit;
        }
        p += 0x11Cu;
        bit <<= 1;
    } while (m != 0);
}

/* src/func_8008ABF0.c: m = D_800BCD50, first bit 0x1000, records at
 * D_800BC000 + 0xF4. */
/* func_8008ABF0: ported from the matching decomp -- generated TU pc_port/game/decomp/func_8008ABF0_port.c (src/func_8008ABF0.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* src/func_8008AB9C.c: m = D_8009D2C8[1], first bit 1, records at a0 + 0xF4. */
/* func_8008AB9C: ported from the matching decomp -- generated TU pc_port/game/decomp/func_8008AB9C_port.c (src/func_8008AB9C.c); hand port retired (port3 switch-over F). */

/* src/func_80080CDC.c: dst == NULL -> func_8007FC28(); else copy the 4-byte
 * quad it points to into dst and return dst. */
pe_addr_t func_80080CDC(pe_addr_t dst)
{
    pe_addr_t src;

    if (dst == 0u)
        return func_8007FC28();
    src = func_8007FC28();
    PE_StoreU8(dst + 0u, PE_LoadU8(src + 0u));
    PE_StoreU8(dst + 1u, PE_LoadU8(src + 1u));
    PE_StoreU8(dst + 2u, PE_LoadU8(src + 2u));
    PE_StoreU8(dst + 3u, PE_LoadU8(src + 3u));
    return dst;
}

/* src/func_80083D9C.c: switch on a0[0x46]: 2 -> [0x36] = 0x44,
 * +0x2C = a0 + 0x51, [0x35] = 2; 3 -> [0x36] = 0x4D, +0x2C = a0 + 0x5D,
 * [0x35] = 6; otherwise nothing. */
void func_80083D9C(pe_addr_t a0)
{
    int t = PE_LoadU8(a0 + 0x46u);

    switch (t) {
    case 2:
        PE_StoreU8(a0 + 0x36u, 0x44u);
        PE_StoreU32(a0 + 0x2Cu, a0 + 0x51u);
        PE_StoreU8(a0 + 0x35u, (uint8_t)t);
        break;
    case 3:
        PE_StoreU8(a0 + 0x36u, 0x4Du);
        PE_StoreU32(a0 + 0x2Cu, a0 + 0x5Du);
        PE_StoreU8(a0 + 0x35u, 6u);
        break;
    }
}

/* src/func_80085A04.c: a0 <= 0 -> 0; else a1[0] = 0x40001010,
 * D_8009B464 = a1, D_8009B460 = 0, D_8009B45C = a0,
 * a1[1] = (0x10000 << D_8009B424) - 0x1010; return a0. */
int func_80085A04(int a0, pe_addr_t a1)
{
    int sh;

    if (a0 <= 0)
        return 0;
    sh = (int)PE_LoadU32(0x8009B424u);
    PE_StoreU32(a1, 0x40001010u);
    PE_StoreU32(0x8009B464u, a1);
    PE_StoreU32(0x8009B460u, 0u);
    PE_StoreU32(0x8009B45Cu, (uint32_t)a0);
    PE_StoreU32(a1 + 4u, (uint32_t)((0x10000 << sh) - 0x1010));
    return a0;
}

/* src/func_800DFB20.c: when a0->f14 (s16 +0x14) != 0 and b = a0->f8 has
 * b->f1A (u16 +0x1A) >= b->fF (u8 +0xF): b->f14 (+0x14) = a0->f18
 * (s8 +0x18); then a0->f14-- unless it is -1. */
void func_800DFB20(pe_addr_t a0)
{
    pe_addr_t b = PE_LoadU32(a0 + 8u);
    int16_t f14 = (int16_t)PE_LoadU16(a0 + 0x14u);

    if (f14 != 0) {
        if (PE_LoadU16(b + 0x1Au) >= (unsigned int)PE_LoadU8(b + 0xFu)) {
            PE_StoreU32(b + 0x14u, (uint32_t)(int32_t)(int8_t)PE_LoadU8(a0 + 0x18u));
            if (f14 != -1)
                PE_StoreU16(a0 + 0x14u, (uint16_t)(f14 - 1));
        }
    }
}

/* src/func_800C6584.c: XZ circle overlap of two SVECs:
 * dx^2 + dz^2 < (a1 + a3)^2 (32-bit int arithmetic). */
int func_800C6584(pe_addr_t a0, int a1, pe_addr_t a2, int a3)
{
    int dx = (int16_t)PE_LoadU16(a0 + 0u) - (int16_t)PE_LoadU16(a2 + 0u);
    int dz = (int16_t)PE_LoadU16(a0 + 4u) - (int16_t)PE_LoadU16(a2 + 4u);
    int r = a1 + a3;

    return (int)((uint32_t)dx * (uint32_t)dx + (uint32_t)dz * (uint32_t)dz) <
           (int)((uint32_t)r * (uint32_t)r);
}

/* src/func_800629BC.c: walk the D_8009D154 list (next at word 0); return
 * the first node whose words [2..5] contain a0, else NULL. */
pe_addr_t func_800629BC(int a0)
{
    pe_addr_t node = PE_LoadU32(0x8009D154u);

    while (node != 0u) {
        int i;

        for (i = 0; i < 4; i++)
            if ((int)PE_LoadU32(node + 8u + (uint32_t)i * 4u) == a0)
                break;
        if (i < 4)
            return node;
        node = PE_LoadU32(node);
    }
    return 0u;
}

/* src/func_8006EC84.c: for i < count, func_800718D0(base + *(int *)(base +
 * (short)i * 4)) — an offset table of `count` entries relative to base. */
void func_8006EC84(int base, int count)
{
    int i;

    for (i = 0; i < count; i++)
        (void)func_800718D0((pe_addr_t)(base +
            (int)PE_LoadU32((pe_addr_t)(base + (short)i * 4))));
}

/* src/func_80084644.c: when a0[0x49] != 0, clear the slot state fields
 * (in retail order, including the duplicated +0xE6 halfword clear) and
 * fill the 6 bytes at +0x5D with 0xFF. */
void func_80084644(pe_addr_t a0)
{
    pe_addr_t p;
    int i;

    if (PE_LoadU8(a0 + 0x49u) == 0)
        return;
    p = a0 + 0x5Du;
    PE_StoreU8(a0 + 0x49u, 0u);
    PE_StoreU8(a0 + 0x46u, 0u);
    PE_StoreU16(a0 + 0xE6u, 0u);
    PE_StoreU32(a0 + 0x14u, 0u);
    PE_StoreU32(a0 + 0x18u, 0u);
    PE_StoreU8(a0 + 0xE3u, 0u);
    PE_StoreU8(a0 + 0xE4u, 0u);
    PE_StoreU16(a0 + 0xE6u, 0u);
    PE_StoreU8(a0 + 0xE9u, 0u);
    PE_StoreU8(a0 + 0xEAu, 0u);
    PE_StoreU32(a0 + 0x0u, 0u);
    PE_StoreU32(a0 + 0x4u, 0u);
    PE_StoreU32(a0 + 0x8u, 0u);
    for (i = 5; i >= 0; i--)
        PE_StoreU8(p++, 0xFFu);
}

/* src/func_8008B168.c: base = a0->+4; p1 = base[0] (u16) != 0xFFFF ?
 * base + 4 + it : 0; p2 likewise from base[1]; a0->+4 = a0->+0x14; then
 * func_8008A92C(a0, p1, p2) (generated TU, native since 2026-10-07). */
void func_8008B168(pe_addr_t a0)
{
    pe_addr_t base = PE_LoadU32(a0 + 4u);
    unsigned int p1 = PE_LoadU16(base);
    unsigned int o = PE_LoadU16(base + 2u);
    unsigned int p2 = 0;

    p1 = (p1 != 0xFFFFu) ? p1 + base + 4u : 0u;
    if (o != 0xFFFFu)
        p2 = o + PE_LoadU32(a0 + 4u) + 4u;
    PE_StoreU32(a0 + 4u, PE_LoadU32(a0 + 0x14u));
    func_8008A92C((pe_addr_t)(a0), (int)(p1), (int)(p2));
}

/* ── callers of the batch 1–5 adapters that the generator cannot derive ── */

extern void func_80089218(pe_addr_t pe_a0, unsigned int a1);
extern unsigned char func_8007FC18(void);   /* generated TU (src/func_8007FC18.c) */
extern unsigned char func_8007FC44(void);   /* generated TU (src/func_8007FC44.c) */

/* src/func_80082204.c: func_800824C8(0); D_8009B6EC =
 * CdPosToInt(func_8007FC28()); then func_8007F0C8(FC18 & 0xFF,
 * func_8007FC28(), FC44 & 0xFF, func_8008227C, -1) — the callback is the
 * retail text address of func_8008227C. */
void func_80082204(void)
{
    uint32_t a, c;
    pe_addr_t b;

    (void)func_800824C8(0u);
    PE_StoreU32(0x8009B6ECu, (uint32_t)func_80080C48(func_8007FC28()));
    a = func_8007FC18() & 0xFFu;
    b = func_8007FC28();
    c = func_8007FC44() & 0xFFu;
    (void)func_8007F0C8(a, b, (int)c, 0x8008227Cu, 0xFFFFFFFFu);
}

/* src/func_80089250.c: m = (rec[1] & rec[3]) | (rec[0x1B] & rec[0x1D]) | a0
 * over the D_8009D2C8 record words (+4/+0xC/+0x6C/+0x74).  For each of 24
 * voices: masked -> D_800B002C[i*4] = 0x7FFF; otherwise read the voice's
 * +0xC register into D_800B002C[i*4] (func_80089F08) and, when it reads 0,
 * func_80089218 on both D_800B8AC0 banks (+0 and +0x1AA0). */
/* func_80089250: ported from the matching decomp -- generated TU pc_port/game/decomp/func_80089250_port.c (src/func_80089250.c); hand port retired (port3 switch-over F). */

/* src/func_800CCFA0.c / 800CD980.c: advance the overlay's sequence cursor
 * (slot 0xD of the func_800C2B10 table) through the pointer list at
 * *func_800C2B28(1); flag slot 0xE when the next entry is NULL; publish the
 * current entry to `cur` and its +0x268/+0x26A/+0x26C halfwords to the
 * three globals; optionally play D_800B0E14; then func_800CEDA8(0). */
static void seq_advance(pe_addr_t cur, pe_addr_t g0, pe_addr_t g1, pe_addr_t g2)
{
    pe_addr_t p = func_800C2B10(0xD);
    int i = (int)PE_LoadU32(p);
    pe_addr_t base = PE_LoadU32(func_800C2B28(1));
    pe_addr_t t;

    PE_StoreU32(cur, PE_LoadU32(base + (uint32_t)i * 4u));
    i++;
    if (PE_LoadU32(base + (uint32_t)i * 4u) == 0u)
        PE_StoreU32(func_800C2B10(0xE), 1u);
    PE_StoreU32(func_800C2B10(0xD), (uint32_t)i);
    t = PE_LoadU32(cur);
    PE_StoreU16(g0, PE_LoadU16(t + 0x268u));
    PE_StoreU16(g1, PE_LoadU16(t + 0x26Au));
    PE_StoreU16(g2, PE_LoadU16(t + 0x26Cu));
    if (PE_LoadU8(0x800B0CE8u) != 0)
        (void)func_80086608(PE_LoadU32(0x800B0E14u), 0u, 0x80u, 0x7Fu);
    func_800CEDA8(0);
}
void func_800CCFA0(void)
{
    seq_advance(0x800E2810u, 0x800E27F8u, 0x800E27FAu, 0x800E27FCu);
}
void func_800CD980(void)
{
    seq_advance(0x800E2804u, 0x800E27F0u, 0x800E27F2u, 0x800E27F4u);
}
