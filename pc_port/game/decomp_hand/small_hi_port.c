/*
 * Hand adapters — small getters / setters / thin wrappers (>= 0x80060000).
 *
 * Bodies follow the matched leaves src/func_XXXXXXXX.c.  Pointer values are
 * 32-bit guest addresses; function-pointer globals hold retail text
 * addresses (stored and returned as words, never called from here).
 */
#include "pe_guest_decomp.h"
#include "hand_hi_protos.h"

extern int func_8007B010(uint32_t cmdi, pe_addr_t buf);
extern int func_8007C044(int a0, unsigned int a1);
extern void func_80090F68(pe_addr_t pe_a0, unsigned int a1);

/* src/func_8007A354.c: return &D_8009AFD0. */
/* func_8007A354: ported from the matching decomp -- generated TU pc_port/game/decomp/func_8007A354_port.c (src/func_8007A354.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* src/func_8007FC28.c: return &D_8009B582. */
/* func_8007FC28: ported from the matching decomp -- generated TU pc_port/game/decomp/func_8007FC28_port.c (src/func_8007FC28.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* src/func_8007FBC0.c: D_800A36A0 = cb. */
/* func_8007FBC0: ported from the matching decomp -- generated TU pc_port/game/decomp/func_8007FBC0_port.c (src/func_8007FBC0.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* src/func_8007A4A8.c: exchange the D_8009AFB4 callback, return the old one. */
/* func_8007A4A8: ported from the matching decomp -- generated TU pc_port/game/decomp/func_8007A4A8_port.c (src/func_8007A4A8.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* src/func_80081254.c: exchange the D_8009B6D0 callback, return the old one. */
/* func_80081254: ported from the matching decomp -- generated TU pc_port/game/decomp/func_80081254_port.c (src/func_80081254.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* src/func_800C2B10.c: return D_800E2248 + arg0 * 4 + 8. */
/* func_800C2B10: ported from the matching decomp -- generated TU pc_port/game/decomp/func_800C2B10_port.c (src/func_800C2B10.c); hand port retired (port3 switch-over C). */

/* src/func_800C2B28.c: return D_800E2248 + arg0 * 4 + 0x48. */
/* func_800C2B28: ported from the matching decomp -- generated TU pc_port/game/decomp/func_800C2B28_port.c (src/func_800C2B28.c); hand port retired (port3 switch-over C). */

/* src/func_80085728.c: D_8009D240 = base; D_8009D260 = base + 0x800. */
void func_80085728(pe_addr_t base)
{
    PE_StoreU32(0x8009D240u, base);
    PE_StoreU32(0x8009D260u, base + 0x800u);
}

/* src/func_80071944.c: flags (+4) bit 3 set -> &arg0->value (+0xC), else 0. */
pe_addr_t func_80071944(pe_addr_t arg0)
{
    return (PE_LoadU32(arg0 + 4u) & 8u) ? arg0 + 0xCu : 0u;
}

/* src/func_800719C4.c: flags (+4) bit 3 set -> &arg0->value (+0x14), else 0. */
pe_addr_t func_800719C4(pe_addr_t arg0)
{
    return (PE_LoadU32(arg0 + 4u) & 8u) ? arg0 + 0x14u : 0u;
}

/* src/func_8007A468.c declares func_8007B010(void) and calls it bare; the
 * retail body (0x8007A468: `jal 0x8007b010` with only sp/ra touched)
 * forwards the caller's live $a0/$a1, spelled here as explicit arguments. */
void func_8007A468(uint32_t a0, pe_addr_t a1)
{
    (void)func_8007B010(a0, a1);
}

/* src/func_8007A8CC.c: return !func_8007C044(); $a0/$a1 forwarded as above
 * (0x8007A8CC: `jal 0x8007c044`, then `sltiu v0,v0,1`). */
int func_8007A8CC(int a0, unsigned int a1)
{
    return !func_8007C044(a0, a1);
}

/* src/func_80091080.c: func_80090F68(); $a0/$a1 forwarded as above
 * (0x80091080: `jal 0x80090f68` with only sp/ra touched). */
void func_80091080(pe_addr_t a0, unsigned int a1)
{
    func_80090F68(a0, a1);
}

/* src/func_8007FBCC.c / 8007FBD8 / 8007FBE4: D_800A36A4 / A8 / AC = callback. */
/* func_8007FBCC: ported from the matching decomp -- generated TU pc_port/game/decomp/func_8007FBCC_port.c (src/func_8007FBCC.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */
/* func_8007FBD8: ported from the matching decomp -- generated TU pc_port/game/decomp/func_8007FBD8_port.c (src/func_8007FBD8.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */
/* func_8007FBE4: ported from the matching decomp -- generated TU pc_port/game/decomp/func_8007FBE4_port.c (src/func_8007FBE4.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* src/func_80074330.c / 800744A4 / 8007474C: zero arg1 words from arg0
 * (arg1 == 0 is a no-op; the counter runs arg1-1 .. -1). */
static void zero_words(pe_addr_t arg0, int arg1)
{
    int v0;

    if (arg1 == 0)
        return;
    v0 = arg1 - 1;
    do {
        PE_StoreU32(arg0, 0u);
        v0--;
        arg0 += 4u;
    } while (v0 != -1);
}
/* func_80074330: ported from the matching decomp -- generated TU pc_port/game/decomp/func_80074330_port.c (src/func_80074330.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */
/* func_800744A4: ported from the matching decomp -- generated TU pc_port/game/decomp/func_800744A4_port.c (src/func_800744A4.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */
/* func_8007474C: ported from the matching decomp -- generated TU pc_port/game/decomp/func_8007474C_port.c (src/func_8007474C.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* src/func_80077A28.c: fill arg2 bytes at arg0 with arg1 (0 is a no-op). */
/* func_80077A28: ported from the matching decomp -- generated TU pc_port/game/decomp/func_80077A28_port.c (src/func_80077A28.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* src/func_80077A00.c: func_80073CF4(2, func_80076EE4) — the handler is the
 * retail text address of func_80076EE4. */
/* func_80077A00: ported from the matching decomp -- generated TU pc_port/game/decomp/func_80077A00_port.c (src/func_80077A00.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* src/func_80082ADC.c: p = D_800A5AB4; p[0] = func_80082B70; p[1] =
 * func_80082B08 (retail text addresses); p[-1] = 0; p[2] = 0. */
/* func_80082ADC: ported from the matching decomp -- generated TU pc_port/game/decomp/func_80082ADC_port.c (src/func_80082ADC.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* src/func_800C7D00.c and its seven byte-identical twins (800C8E44,
 * 800C9B3C, 800CA6D4, 800CBEE0, 800CCEBC, 800CD89C, 800CE118): forward to
 * func_800C2AF0 and return 0.  src/func_800C2AF0.c reads only its first four
 * parameters, so arg4/arg5 carry no behaviour. */
int func_800C7D00(pe_addr_t base, int unused, int index, unsigned int value,
                  int arg4, int arg5)
{
    (void)arg4; (void)arg5;
    (void)func_800C2AF0(base, unused, index, value);
    return 0;
}

int func_800C8E44(pe_addr_t base, int unused, int index, unsigned int value,
                  int arg4, int arg5)
{
    (void)arg4; (void)arg5;
    (void)func_800C2AF0(base, unused, index, value);
    return 0;
}

int func_800C9B3C(pe_addr_t base, int unused, int index, unsigned int value,
                  int arg4, int arg5)
{
    (void)arg4; (void)arg5;
    (void)func_800C2AF0(base, unused, index, value);
    return 0;
}

int func_800CA6D4(pe_addr_t base, int unused, int index, unsigned int value,
                  int arg4, int arg5)
{
    (void)arg4; (void)arg5;
    (void)func_800C2AF0(base, unused, index, value);
    return 0;
}

int func_800CBEE0(pe_addr_t base, int unused, int index, unsigned int value,
                  int arg4, int arg5)
{
    (void)arg4; (void)arg5;
    (void)func_800C2AF0(base, unused, index, value);
    return 0;
}

int func_800CCEBC(pe_addr_t base, int unused, int index, unsigned int value,
                  int arg4, int arg5)
{
    (void)arg4; (void)arg5;
    (void)func_800C2AF0(base, unused, index, value);
    return 0;
}

int func_800CD89C(pe_addr_t base, int unused, int index, unsigned int value,
                  int arg4, int arg5)
{
    (void)arg4; (void)arg5;
    (void)func_800C2AF0(base, unused, index, value);
    return 0;
}

int func_800CE118(pe_addr_t base, int unused, int index, unsigned int value,
                  int arg4, int arg5)
{
    (void)arg4; (void)arg5;
    (void)func_800C2AF0(base, unused, index, value);
    return 0;
}

/* src/func_80071964.c / 80071994: p = a0 + 8, advanced by the word at +8
 * when flags (+4) bit 3 is set; return p + 4 / p + 12. */
static pe_addr_t buffer_payload(pe_addr_t a0)
{
    pe_addr_t p = a0 + 8u;

    if (PE_LoadU32(a0 + 4u) & 8u)
        p += PE_LoadU32(a0 + 8u);
    return p;
}
pe_addr_t func_80071964(pe_addr_t a0) { return buffer_payload(a0) + 4u; }
pe_addr_t func_80071994(pe_addr_t a0) { return buffer_payload(a0) + 12u; }

extern int func_8007BF44(unsigned int a0, unsigned int a1);   /* generated TU */

/* src/func_8007A8AC.c: return !func_8007BF44(); the caller's live
 * $a0/$a1 are forwarded (0x8007A8AC: `jal 0x8007bf44` with only sp/ra
 * touched), exactly as func_8007A8CC does for func_8007C044. */
int func_8007A8AC(unsigned int a0, unsigned int a1)
{
    return !func_8007BF44(a0, a1);
}
