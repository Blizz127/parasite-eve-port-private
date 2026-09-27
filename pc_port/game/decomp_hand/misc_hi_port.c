/*
 * Hand adapters — small state / copy / conversion leaves (>= 0x80060000).
 *
 * Bodies follow the matched leaves src/func_XXXXXXXX.c; pointers are guest
 * addresses, data symbols are guest-RAM words at their retail addresses.
 */
#include "pe_guest_decomp.h"
#include "pe_sdk.h"
#include "hand_hi_protos.h"

extern void func_80085F44(int a0);
extern void func_80085F74(pe_addr_t pe_a0);   /* generated TU (src/func_80085F74.c) */

/* src/func_8008D820.c: word copy of arg2 >> 2 words from arg0 to arg1 as a
 * do/while — retail copies one word even when arg2 >> 2 == 0 and then runs
 * the counter through zero (reproduced: the count is unsigned). */
void func_8008D820(pe_addr_t arg0, pe_addr_t arg1, unsigned int arg2)
{
    arg2 >>= 2;
    do {
        uint32_t v0 = PE_LoadU32(arg0);
        arg0 += 4u;
        arg2--;
        PE_StoreU32(arg1, v0);
        arg1 += 4u;
    } while (arg2);
}

/* src/func_800850C0.c: D_8009D24C = 1; func_80085F44(func_80085098) — the
 * callback argument is the retail text address of func_80085098. */
void func_800850C0(void)
{
    PE_StoreU32(0x8009D24Cu, 1u);
    func_80085F44((int)0x80085098u);
}

/* src/func_80080B44.c (CdIntToPos): signed int math on arg0 + 150; bytes
 * [2] frames, [1] seconds, [0] minutes, each packed BCD (d/10)*16 + d%10. */
pe_addr_t func_80080B44(int arg0, pe_addr_t arg1)
{
    int t = arg0 + 0x96;
    int q = t / 75;
    int s = t % 75;
    int h = q / 60;
    int m = q % 60;

    PE_StoreU8(arg1 + 2u, (uint8_t)((s / 10) * 16 + s % 10));
    PE_StoreU8(arg1 + 1u, (uint8_t)((m / 10) * 16 + m % 10));
    PE_StoreU8(arg1 + 0u, (uint8_t)((h / 10) * 16 + h % 10));
    return arg1;
}

/* src/func_8007FCBC.c: if D_8009B598 > 0 return 0; else D_8009B570
 * (= &D_8009B598 - 10 words) = 0x20 and return func_8007FCFC((u8)a0).
 * The leaf never writes $a1 (disassembly 0x8007FCBC..0x8007FCF8: only v0,
 * v1, a0 are written), so retail forwards the caller's $a1 as
 * func_8007FCFC's second argument — spelled here as the explicit a1. */
int func_8007FCBC(int a0, int a1)
{
    if ((int)PE_LoadU32(0x8009B598u) > 0)
        return 0;
    PE_StoreU32(0x8009B598u - 40u, 0x20u);
    return func_8007FCFC((unsigned char)a0, (uint32_t)a1);
}

/* ── callers re-adapted once func_8008D820 / func_80080B44 became real ── */

/* src/func_8008AF08.c: when D_8009D2C8->+4 != 0, copy 0x68 bytes from the
 * record at D_8009D2C8 to D_800B8968 and 0x1AA0 bytes D_800B8AC0 ->
 * D_800B6B80; if bit 0x100 of D_800B8968's first word is set, walk 0x18
 * entries (stride 0x11C) from D_800B6B80 + 0x5A lowering every halfword
 * >= 0x50 by 0x30. */
void func_8008AF08(void)
{
    pe_addr_t rec = PE_LoadU32(0x8009D2C8u);
    pe_addr_t p, q;
    unsigned int i;
    unsigned short v;

    if (PE_LoadU32(rec + 4u) == 0u)
        return;
    p = 0x800B8968u;
    func_8008D820(rec, p, 0x68u);
    func_8008D820(0x800B8AC0u, 0x800B6B80u, 0x1AA0u);
    if ((PE_LoadU32(p) & 0x100u) == 0u)
        return;
    i = 0;
    q = 0x800B6B80u + 0x5Au;
    do {
        v = PE_LoadU16(q);
        if (v >= 0x50)
            PE_StoreU16(q, (unsigned short)(v - 0x30));
        i++;
        q += 0x11Cu;
    } while (i < 0x18u);
}

/* src/func_8008AFB8.c: back up the D_8009D2C8 record (0x68 bytes to +0x68)
 * and D_800B8AC0 (0x1AA0 bytes to +0x1AA0) when +4 != 0 and +0x6C == 0,
 * then func_8008A068(a0->+4) and D_8009D2C8->+0x54 = (short)a0->+0xC.
 * func_8008A068 has no pc_port implementation: loud boundary. */
void func_8008AFB8(pe_addr_t a0)
{
    pe_addr_t p = PE_LoadU32(0x8009D2C8u);

    if (PE_LoadU32(p + 4u) != 0u && PE_LoadU32(p + 0x6Cu) == 0u) {
        func_8008D820(p, p + 0x68u, 0x68u);
        func_8008D820(0x800B8AC0u, 0x800B8AC0u + 0x1AA0u, 0x1AA0u);
    }
    PE_D_COMP_BOUNDARY1("func_8008A068", 0x8008A068u, (int)PE_LoadU32(a0 + 4u));
    PE_StoreU16(PE_LoadU32(0x8009D2C8u) + 0x54u,
                (uint16_t)PE_LoadU32(a0 + 0xCu));
}

/* src/func_80081D18.c keeps `int buf[2]` on its own stack and passes its
 * address to func_80080B44 and func_80080E34.  A host stack address has no
 * guest meaning, so the 8 bytes live at this top-of-RAM (retail stack
 * region) scratch address, following PE_7F0C8_LOC / PE_698D4_CDLFILE. */
#define PE_81D18_BUF 0x801FFE40u

/* src/func_80081D18.c: pos = CdIntToPos(a1); func_80080E34(pos, a0, a2,
 * 0x80); poll func_800811E4(0) while > 0; return r == 0.
 * func_80080E34 has no pc_port implementation: loud boundary. */
int func_80081D18(int a0, int a1, int a2)
{
    int r;

    func_80080B44(a1, PE_81D18_BUF);
    PE_D_COMP_BOUNDARY4("func_80080E34", 0x80080E34u, PE_81D18_BUF, a0, a2, 0x80);
    do {
        r = func_800811E4(0u);
    } while (r > 0);
    return r == 0;
}

/* src/func_8008D7D0.c: seed the attribute block at D_800C0D90 (mask 0x1C0,
 * +0x14 = 0, +0x12/+0x10 = D_8009D2B6) and apply it via func_80085F74(p). */
void func_8008D7D0(void)
{
    const pe_addr_t p = 0x800C0D90u;
    unsigned short v = PE_LoadU16(0x8009D2B6u);

    PE_StoreU32(p, 0x1C0u);
    PE_StoreU32(0x800C0DA4u, 0u);
    PE_StoreU16(0x800C0DA2u, v);
    PE_StoreU16(0x800C0DA0u, v);
    func_80085F74(p);
}
