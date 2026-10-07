/*
 * Hand adapters — AKAO sound-driver tick (port lane, port3, 2026-09-28).
 *
 * Line-for-line translations of the matched leaves
 *   src/func_8008DB7C.c  driver tick (tempo, 3 voice banks, queue, SPU flush)
 *   src/func_8008E8D0.c  sequencer step (opcode loop + note dispatch)
 *   src/func_80089328.c  key-on/off + voice parameter flush
 *   src/func_80088980.c  per-voice vibrato/tremolo/pan LFO + volume/pitch mix
 *   src/func_80087AA8.c  per-voice slide / LFO-depth tick
 * The generator rejects them (guest struct pointers, pointer-to-pointer
 * parameters, asm-label alias, callee arity).  Guest objects are 32-bit guest
 * addresses and every field access is a PE_Load/PE_Store at the leaf's own
 * offset and width.  D_8009D2C8 (the active sequence-control record) is
 * re-loaded wherever the leaf re-loads it, because the callees move it.
 * Opcode handlers (D_8009CCF0 / D_8009C8F0) are called through PE_GuestCall
 * with the handler address read from guest RAM, as the retail `jalr` does.
 *
 * Runtime: this native tick is the ONLY AKAO tick in the port binary
 * (platform/pe_audio_driver.c calls it at 240 Hz; native only, 2026-10-07).
 * Verification: PE_AKAO_VERIFY=1 (pe_libetc.c) runs every driver tick once in
 * the test-only retail-code oracle and once natively from the same guest
 * RAM + SPU register snapshot, and compares both (needs a binary that links
 * tests/pe_akao_interp_oracle.c).
 */
#include "pe_guest_decomp.h"
#include "pe_spu_dma.h"
#include <stdio.h>
#include <stdlib.h>

/* Callees with no canonical header prototype (generated TUs; signatures
 * copied from those definitions). */
void func_80089960(void);
void func_80089CF0(void);
void func_80089B28(void);
void func_80089F28(unsigned short a, unsigned short b);
int  func_80089EB8(int a0);
void func_8008F0D0(pe_addr_t pe_a0, pe_addr_t pe_a1, int a2);
void func_8008E7F4(pe_addr_t pe_a0, int a1);

#define AK_MUL(a, b)  ((int32_t)((uint32_t)(int32_t)(a) * (uint32_t)(int32_t)(b)))
#define AK_SHL(a, n)  ((int32_t)((uint32_t)(int32_t)(a) << (n)))

/* Voice-relative accessors (v = guest address of a 0x11C-byte voice). */
#define VA(o)      (v + (uint32_t)(int32_t)(o))
#define LU8(o)     PE_LoadU8(VA(o))
#define LU16(o)    PE_LoadU16(VA(o))
#define LS16(o)    ((int32_t)(int16_t)PE_LoadU16(VA(o)))
#define LU32(o)    PE_LoadU32(VA(o))
#define LI32(o)    ((int32_t)PE_LoadU32(VA(o)))
#define SU16(o, x) PE_StoreU16(VA(o), (uint16_t)(x))
#define SU32(o, x) PE_StoreU32(VA(o), (uint32_t)(x))
#define S16AT(a)   ((int32_t)(int16_t)PE_LoadU16(a))

/* Sequence-control record (0x68 bytes) array at *D_8009D2C8. */
#define GA_D2C8        0x8009D2C8u
#define CTL            PE_LoadU32(GA_D2C8)
#define CTL_SIZE       0x68u
#define VOICE_SIZE     0x11Cu
#define GA_BANK0       0x800B8AC0u   /* D_800B8AC0: 24 voices, sequence 0 */
#define GA_BANK1       0x800BA560u   /* D_800BA560: 24 voices, sequence 1 */
#define GA_BANKSFX     0x800BC000u   /* D_800BC000: SFX voices (bit 0x1000..) */

#define G32(a)        PE_LoadU32(a)
#define S32(a, x)     PE_StoreU32((a), (uint32_t)(x))
#define OR32(a, x)    S32((a), G32(a) | (uint32_t)(x))
#define AND32(a, x)   S32((a), G32(a) & (uint32_t)(x))

/* Scratch for func_80089328's stack local `flags` (func_8008900C takes its
 * address).  Unused by every other hand window (0x801FF500.. and up). */
#define PE_HAND_AKAO_STACK 0x801FF4F0u

/* Call counters (PE_AKAO_VERIFY summary only). */
static unsigned long g_ak_n88980, g_ak_n87aa8, g_ak_ne8d0, g_ak_nkon;

/* ── src/func_80088980.c ─────────────────────────────────────────────── */
static int32_t ak_wave_step(pe_addr_t v, uint32_t slot)
{
    /* t = PTR(slot); if (t[0] == 0 && t[1] == 0) PTR(slot) = t + t[2];
     * s = *PTR(slot)++; */
    pe_addr_t t = LU32(slot);
    int32_t s;
    if (S16AT(t) == 0 && S16AT(t + 2u) == 0)
        SU32(slot, t + (uint32_t)AK_MUL(S16AT(t + 4u), 2));
    t = LU32(slot);
    s = S16AT(t);
    SU32(slot, t + 2u);
    return s;
}

void func_80088980(pe_addr_t v, unsigned int mask)
{
    int32_t vol, r, pan = 0, p, k, s;
    uint32_t fl;
    uint16_t cnt;

    g_ak_n88980++;

    vol = AK_MUL(LS16(0x46), LU16(0x6C) >> 8) >> 7;
    fl = LU32(0x38);
    if (fl & 1u) {
        cnt = (uint16_t)(LU16(0x8E) - 1u);
        SU16(0x8E, cnt);
        if (cnt == 0) {
            SU16(0x8E, LU16(0x8C));
            s = ak_wave_step(v, 0x1Cu);
            r = AK_MUL(LU16(0x92), s) >> 16;
            if (r != LS16(0xE8)) {
                SU16(0xE8, r);
                SU32(0xF4, LU32(0xF4) | 0x10u);
                if (r >= 0)
                    SU16(0xE8, AK_MUL(r, 2));
            }
        }
    }
    if (fl & 2u) {
        cnt = (uint16_t)(LU16(0xA2) - 1u);
        SU16(0xA2, cnt);
        if (cnt == 0) {
            int32_t x, y;
            SU16(0xA2, LU16(0xA0));
            s = ak_wave_step(v, 0x20u);
            y = AK_SHL(AK_MUL(vol, LU16(0xA6) >> 8), 9);
            x = y >> 16;
            r = AK_MUL(x, s) >> 15;
            if (r != LS16(0xEA)) {
                SU16(0xEA, r);
                SU32(0xF4, LU32(0xF4) | 3u);
            }
        }
    }
    if (fl & 4u) {
        cnt = (uint16_t)(LU16(0xB0) - 1u);
        SU16(0xB0, cnt);
        if (cnt == 0) {
            SU16(0xB0, LU16(0xAE));
            s = ak_wave_step(v, 0x24u);
            r = AK_MUL(LU16(0xB4) >> 8, s) >> 15;
            if (r != LS16(0xEC)) {
                SU16(0xEC, r);
                SU32(0xF4, LU32(0xF4) | 3u);
            }
        }
    }
    if (fl & 0x20u) {
        vol = AK_MUL((int16_t)(uint16_t)(LU16(-0x10) << 1), LU16(0x6C) >> 8) >> 7;
        SU32(0xF4, LU32(0xF4) | 3u);
    }
    if (LU32(0xF4) & 3u) {
        vol += LS16(0xEA);
        if (!(LU32(0x2C) & 0x2000000u)) {
            vol = AK_MUL(vol, (int8_t)(uint8_t)(LU16(0xD8) >> 8)) >> 7;
            pan = (int32_t)(((LU16(0x76) >> 8) + (uint32_t)LS16(0xEC)) & 0xFFu);
        } else {
            pan = 0x80;
        }
        switch ((int32_t)G32(0x8009D2C0u)) {
        case 1:
            SU16(0x118, AK_MUL(vol, S16AT(0x8009B8F8u + (uint32_t)pan * 2u)) >> 15);
            SU16(0x11A, AK_MUL(vol, S16AT(0x8009B8F8u + (uint32_t)(pan ^ 0xFF) * 2u)) >> 15);
            break;
        case 4:
            SU16(0x118, AK_MUL(vol, S16AT(0x8009B8F8u + (uint32_t)pan * 2u)) >> 15);
            r = AK_MUL(vol, S16AT(0x8009B8F8u + (uint32_t)(pan ^ 0xFF) * 2u)) >> 15;
            SU16(0x11A, r);
            if (mask & 0xAAAAAAu)
                SU16(0x11A, ~r);
            else
                SU16(0x118, ~(uint32_t)LU16(0x118));
            break;
        default:
            SU16(0x11A, (uint32_t)AK_MUL(vol, S16AT(0x8009B9F8u)) >> 15);
            SU16(0x118, LS16(0x11A));
            break;
        }
    }
    if (fl & 0x10u) {
        p = (int32_t)LU16(-0x10) + LS16(0xE8) + LS16(0x36);
        if (!(LU32(0x2C) & 0x2000000u) && (k = LU8(0x3D)) != 0) {
            if (k < 0x80)
                p += AK_MUL(p, k) >> 7;
            else
                p = AK_MUL(p, k) >> 8;
        }
        SU16(0x10C, p & 0x3FFF);
        SU32(0xF4, LU32(0xF4) | 0x10u);
    } else if (LU32(0xF4) & 0x10u) {
        p = LI32(0x30) + LS16(0xE8) + LS16(0x36);
        if (!(LU32(0x2C) & 0x2000000u) && (k = LU8(0x3D)) != 0) {
            if (k < 0x80)
                p += AK_MUL(p, k) >> 7;
            else
                p = AK_MUL(p, k) >> 8;
        }
        SU16(0x10C, p & 0x3FFF);
    }
}

/* ── src/func_80087AA8.c ─────────────────────────────────────────────── */
/* p = v->wave; if (p[0] == 0 && p[1] == 0) p += p[2]; return *p;
 * (a local walk: the voice's wave pointer is not advanced here). */
static int32_t ak_wave_peek(pe_addr_t v, uint32_t slot)
{
    pe_addr_t p = LU32(slot);
    if (S16AT(p) == 0 && S16AT(p + 2u) == 0)
        p += (uint32_t)AK_MUL(S16AT(p + 4u), 2);
    return S16AT(p);
}

static int ak_dec16(pe_addr_t a)   /* --u16; returns the new value */
{
    uint16_t n = (uint16_t)(PE_LoadU16(a) - 1u);
    PE_StoreU16(a, n);
    return n;
}

void func_80087AA8(pe_addr_t v, unsigned int mask)
{
    int32_t n;
    uint32_t d, b;

    g_ak_n87aa8++;

    if (LU16(0x72)) {                                   /* c72: volume slide */
        ak_dec16(VA(0x72));
        n = LI32(0x44) + LI32(0x48);
        if (((uint32_t)n & 0xFFE00000u) != (LU32(0x44) & 0xFFE00000u))
            SU32(0xF4, LU32(0xF4) | 3u);
        SU32(0x44, n);
    }
    if (LU16(0x60)) {
        ak_dec16(VA(0x60));
        SU16(0x5E, LU16(0x5E) + LU16(0xD6));
        SU32(0xF4, LU32(0xF4) | 3u);
    }
    if (LU16(0x6E)) {                                   /* vol_scale slide */
        ak_dec16(VA(0x6E));
        n = (int32_t)LU16(0x6C) + LS16(0xD4);
        if ((n & 0x7F00) != (int32_t)(LU16(0x6C) & 0x7F00u))
            SU32(0xF4, LU32(0xF4) | 3u);
        SU16(0x6C, n);
    }
    if (LU16(0x74)) {                                   /* expression slide */
        ak_dec16(VA(0x74));
        n = LS16(0xD8) + LS16(0xDA);
        if ((LU32(0x38) & 0x100u) && (n & 0xFF00) != (LS16(0xD8) & 0xFF00))
            SU32(0xF4, LU32(0xF4) | 3u);
        SU16(0xD8, n);
    }
    if (LU16(0x78)) {                                   /* pan slide */
        ak_dec16(VA(0x78));
        n = (int32_t)LU16(0x76) + LS16(0xDC);
        if ((n & 0xFF00) != (int32_t)(LU16(0x76) & 0xFF00u))
            SU32(0xF4, LU32(0xF4) | 3u);
        SU16(0x76, n);
    }
    if (LU16(0x8A))
        ak_dec16(VA(0x8A));
    if (LU16(0x9E))
        ak_dec16(VA(0x9E));
    if (LU16(0xBA)) {
        if (ak_dec16(VA(0xBA)) == 0) {
            pe_addr_t c = CTL;
            S32(c + 0x34u, G32(c + 0x34u) ^ mask);
            OR32(0x8009D2C4u, 0x10u);
            func_80089960();
        }
    }
    if (LU16(0xBC)) {
        if (ak_dec16(VA(0xBC)) == 0) {
            pe_addr_t c = CTL;
            S32(c + 0x3Cu, G32(c + 0x3Cu) ^ mask);
            func_80089CF0();
        }
    }
    if (LU16(0x96)) {                                   /* vibrato depth slide */
        ak_dec16(VA(0x96));
        SU16(0x94, LU16(0x94) + LU16(0x98));
        d = (LU16(0x94) & 0x7F00u) >> 8;
        if (LU16(0x94) & 0x8000u)
            b = (d * LU32(0x30)) >> 7;
        else
            b = (d * ((LU32(0x30) * 15u) >> 8)) >> 7;
        SU16(0x92, b);
        if (LU16(0x8A) == 0 && LU16(0x8E) != 1) {
            n = AK_MUL(LU16(0x92), ak_wave_peek(v, 0x1Cu)) >> 16;
            if (n != LS16(0xE8)) {
                SU16(0xE8, n);
                SU32(0xF4, LU32(0xF4) | 0x10u);
                if (n >= 0)
                    SU16(0xE8, AK_SHL(n, 1));
            }
        }
    }
    if (LU16(0xA8)) {                                   /* tremolo depth slide */
        ak_dec16(VA(0xA8));
        SU16(0xA6, LU16(0xA6) + LU16(0xAA));
        if (LU16(0x9E) == 0 && LU16(0xA2) != 1) {
            int32_t w = ak_wave_peek(v, 0x20u);
            n = (int16_t)(AK_MUL(AK_MUL(LS16(0x46), LU16(0x6C) >> 8) >> 7, LU16(0xA6) >> 8) >> 7);
            n = AK_MUL(n, w) >> 15;
            if (n != LS16(0xEA)) {
                SU16(0xEA, n);
                SU32(0xF4, LU32(0xF4) | 3u);
            }
        }
    }
    if (LU16(0xB6)) {                                   /* pan-LFO depth slide */
        ak_dec16(VA(0xB6));
        SU16(0xB4, LU16(0xB4) + LU16(0xB8));
        if (LU16(0xB0) != 1) {
            n = AK_MUL(LU16(0xB4) >> 8, ak_wave_peek(v, 0x24u)) >> 15;
            if (n != LS16(0xEC)) {
                SU16(0xEC, n);
                SU32(0xF4, LU32(0xF4) | 3u);
            }
        }
    }
    if (LU16(0x7A)) {                                   /* pitch slide */
        ak_dec16(VA(0x7A));
        n = LI32(0x34) + LI32(0x4C);
        if (((uint32_t)n & 0xFFFF0000u) != (LU32(0x34) & 0xFFFF0000u))
            SU32(0xF4, LU32(0xF4) | 0x10u);
        SU32(0x34, n);
    }
}

/* ── src/func_80089328.c ─────────────────────────────────────────────── */
void func_80089328(void)
{
    const pe_addr_t flags = PE_HAND_AKAO_STACK;   /* stack local `flags` */
    uint32_t m, a, b, c, d;
    pe_addr_t t, u, w, x, v;
    uint32_t bit;

    S32(flags, 0);
    m = G32(0x800BCD50u) | G32(0x800BCD60u);
    t = CTL;
    if ((G32(t + 4u) & G32(t + 0x10u)) |
        (G32(t + CTL_SIZE + 4u) & G32(t + CTL_SIZE + 0x10u)))
        func_80089250(m);
    t = CTL;
    a = G32(t + CTL_SIZE + 4u) & G32(t + CTL_SIZE + 0x14u) &
        ~(G32(t + CTL_SIZE + 0xCu) & m);
    b = a & G32(t + CTL_SIZE + 0xCu) & ~m;
    if (a & G32(t + CTL_SIZE + 8u)) {
        uint32_t f8 = G32(t + CTL_SIZE + 8u);
        S32(GA_D2C8, t + CTL_SIZE);
        func_8008900C(GA_BANK1, a & f8, b, flags);
        u = CTL;
        a &= ~G32(u + 8u);
        S32(GA_D2C8, u - CTL_SIZE);
        S32(u + 0x10u, G32(u + 0x10u) & ~G32(u + 8u));
    }
    w = CTL;
    c = G32(w + 4u) & G32(w + 0x14u) & ~(G32(w + 0xCu) & (b | m));
    d = c & G32(w + 0xCu) & ~(b | m);
    if (c & G32(w + 8u)) {
        func_8008900C(GA_BANK0, c & G32(w + 8u), d, flags);
        x = CTL;
        c &= ~G32(x + 8u);
        S32(x + 0x10u, G32(x + 0x10u) & ~G32(x + 8u));
    }
    if (a != 0) {
        S32(GA_D2C8, CTL + CTL_SIZE);
        func_8008900C(GA_BANK1, a, b & ~d, flags);
        S32(CTL + 0x10u, 0);
        S32(GA_D2C8, CTL - CTL_SIZE);
    }
    if (c != 0) {
        func_8008900C(GA_BANK0, c, d, flags);
        S32(CTL + 0x10u, 0);
    }
    c = G32(0x800BCD50u) & G32(0x800BCD58u);
    if (c != 0) {
        bit = 0x1000u;
        v = GA_BANKSFX;
        S32(flags, G32(flags) | G32(0x800BCD54u));
        do {
            if (c & bit) {
                func_80088980(v, bit);
                if (G32(v + 0xF4u) != 0) {
                    /* retail passes *(v+0x38) as a third argument; the
                     * matched func_800878F0 takes two. */
                    func_800878F0((int)G32(v + 0xF0u), v + 0xF0u);
                }
                c &= ~bit;
            }
            bit <<= 1;
            v += VOICE_SIZE;
        } while (c != 0);
        S32(0x800BCD54u, 0);
    }
    c = G32(0x8009D2C4u);
    if (c & 0x80u) {
        uint16_t f42 = PE_LoadU16(CTL + 0x42u);
        func_80089F28(f42, f42);
        AND32(0x8009D2C4u, ~0x80u);
    }
    if (c & 0x10u) {
        if (G32(0x800BCD50u) != 0)
            (void)func_80089EB8(PE_LoadU16(0x800BCD78u));
        else
            (void)func_80089EB8(PE_LoadU16(CTL + 0x5Au));
        AND32(0x8009D2C4u, ~0x10u);
    }
    if (c & 0x100u) {
        func_80089B48();
        func_80089980();
        func_80089D10();
        func_80087744(G32(0x800C0DD0u));
        func_80087760(G32(0x800C0DD4u));
        func_8008777C(G32(0x800C0DD8u));
        AND32(0x8009D2C4u, ~0x100u);
    }
    if (G32(flags) != 0) {
        g_ak_nkon++;
        func_8008770C(G32(flags));
    }
}

/* ── src/func_8008E8D0.c ─────────────────────────────────────────────── */
static void ak_rest_gate(pe_addr_t v, uint32_t bit)
{
    /* if (U16(0x54) == 0 && (D_8009D2C8->f14 & bit) && U32(0xF0) < 0x18)
     *     D_8009D2C8->f18 |= bit; */
    if (LU16(0x54) == 0 && (G32(CTL + 0x14u) & bit) && LU32(0xF0) < 0x18u)
        OR32(CTL + 0x18u, bit);
}

void func_8008E8D0(pe_addr_t v, unsigned int bit)
{
    pe_addr_t p, ent, c;
    uint32_t op, k, x, d, m, f, w, y;
    int32_t add, add2, t;

    g_ak_ne8d0++;

    for (;;) {
        p = LU32(0);
        SU32(0, p + 1u);
        op = PE_LoadU8(p);
        if (op >= 0xA0u) {
            if (op == 0xFCu) {
                SU32(0, p + 2u);
                k = PE_LoadU8(p + 1u);
                (void)PE_GuestCall("@8008E8D0:D_8009CCF0", G32(0x8009CCF0u + k * 4u), 2u,
                                   (uintptr_t)v, (uintptr_t)bit, 0, 0);
            } else {
                if (op == 0xCAu && (LU32(0x38) & 0x200000u)) {
                    OR32(0x800BCD5Cu, bit);
                    op = 0xA0u;
                }
                (void)PE_GuestCall("@8008E8D0:D_8009C8F0", G32(0x8009C8F0u + op * 4u), 2u,
                                   (uintptr_t)v, (uintptr_t)bit, 0, 0);
            }
        }
        if (op < 0xA0u || op == 0xA0u)
            break;
    }
    if (op == 0xA0u) {
        ak_rest_gate(v, bit);
        return;
    }
    k = (uint32_t)func_8008E2DC(v) & 0xFFu;
    {
        int32_t a = LS16(0xD2);
        uint16_t t2 = (uint16_t)a;
        if (a != 0) {
            SU16(0x58, t2);
            SU16(0x56, t2);
        }
    }
    if (LU16(0x56) != 0) {
        if (k >= 0x8Fu || (k < 0x84u && !(LU16(0x84) & 5u)))
            SU16(0x58, LU16(0x58) - 2u);
    } else {
        add = PE_LoadU16(0x8009B8DCu + (op % 11u) * 2u);
        SU16(0x56, add);
        t = add;
        if (k - 0x84u >= 0xBu && !(LU16(0x84) & 5u))
            t -= 2;
        SU16(0x58, t);
    }
    SU16(0xD0, LU16(0x56));
    SU32(0xF4, LU32(0xF4) | 0x4000u);
    if (k < 0x8Fu)
        SU32(0x38, LU32(0x38) & ~0x40u);
    else
        SU32(0x38, LU32(0x38) | 0x40u);
    if (op >= 0x8Fu) {
        ak_rest_gate(v, bit);
        SU16(0x82, 0);
        SU16(0xE8, 0);
        SU16(0xEA, 0);
        SU16(0x84, LU16(0x84) & 0xFFFDu);
        return;
    }
    if (op < 0x84u) {
        op = op / 11u;
        if (LU32(0x38) & 8u) {
            c = CTL;
            OR32(c + 0x10u, bit);
            if ((G32(c + 0x14u) & bit) && LU32(0xF0) < 0x18u)
                OR32(c + 0x18u, bit);
            ent = LU32(0x14) + (op % 12u) * 6u;
            f = G32(CTL) & 0x100u;
            add2 = -(f != 0) & 0x30;
            w = LU16(0x5A);
            y = PE_LoadU8(ent);
            if (y < 0x20u) {
                if (w == y)
                    goto same;
            } else if (w == y + (uint32_t)add2) {
                goto reload;
            }
            {
                uint32_t b8 = PE_LoadU8(ent);
                x = b8 & 0xFFu;
                if (b8 >= 0x20u)
                    x += (uint32_t)add2;
            }
            SU16(0x5A, x);
            func_8008F0D0(v, 0x800B2900u + x * 64u, (int)G32(0x800B2900u + x * 64u));
reload:
            w = LU16(0x5A);
same:
            k = func_8008E840((int)w, PE_LoadU8(ent + 1u), LS16(0xE0));
            {
                int32_t s = LU16(0x6A);
                s = AK_MUL(s, (int32_t)PE_LoadU8(ent + 2u) + (int32_t)PE_LoadU8(ent + 3u) * 256);
                SU32(0x44, AK_MUL(s, 4));
            }
            SU16(0x76, ((PE_LoadU8(ent + 4u) + 0x40u) & 0xFFu) << 8);
            if (PE_LoadU8(ent + 5u) != 0)
                OR32(CTL + 0x38u, bit);
            else
                AND32(CTL + 0x38u, ~bit);
            func_80089B28();
        } else {
            op += (uint32_t)LU16(0x7C) * 12u;
            if (!(LU16(0x84) & 2u)) {
                if (LU16(0x54) == 0) {
                    if (LU32(0x38) & 0x1000u)
                        func_8008E7F4(v, (int)op);
                    c = CTL;
                    OR32(c + 0x10u, bit);
                    if ((G32(c + 0x14u) & bit) && LU32(0xF0) < 0x18u)
                        OR32(c + 0x18u, bit);
                } else {
                    OR32(0x800BCD54u, bit);
                }
                SU16(0x7A, 0);
            }
            if (LU16(0x82) != 0 && LU16(0x80) != 0) {
                SU16(0x7E, LU16(0x82));
                SU16(0xE4, (uint32_t)LU16(0xDE) + op - LU16(0x80) - LU16(0xE6));
                SU16(0xE2, (uint32_t)LU16(0x80) - ((uint32_t)LU16(0xDE) - LU16(0xE6)));
                op = (uint32_t)LU16(0x80) + (uint32_t)LS16(0xE6);
            } else {
                SU16(0xE2, op);
                op += (uint32_t)LS16(0xDE);
            }
            k = func_8008E840(LU16(0x5A), op, LS16(0xE0));
        }
        SU32(0x30, k);
        if (LU16(0x54) == 0)
            OR32(CTL + 0x14u, bit);
        else
            OR32(0x800BCD58u, bit);
        SU32(0xF4, LU32(0xF4) | 0x13u);
        op = LU32(0x38);
        if (op & 1u) {
            uint32_t xq = LU16(0x94);
            d = (xq & 0x7F00u) >> 8;
            if (!(xq & 0x8000u))
                m = (d * ((k * 15u) >> 8)) >> 7;
            else
                m = (d * k) >> 7;
            SU16(0x92, m);
            SU32(0x1C, G32(0x8009C080u + (uint32_t)LU16(0x90) * 4u));
            SU16(0x8A, LU16(0x88));
            SU16(0x8E, 1);
        }
        if (op & 2u) {
            SU32(0x20, G32(0x8009C080u + (uint32_t)LU16(0xA4) * 4u));
            SU16(0x9E, LU16(0x9C));
            SU16(0xA2, 1);
        }
        if (op & 4u) {
            SU32(0x24, G32(0x8009C080u + (uint32_t)LU16(0xB2) * 4u));
            SU16(0xB0, 1);
        }
        SU16(0xE8, 0);
        SU16(0xEA, 0);
        SU32(0x34, 0);
    }
    {
        uint32_t xq = LU16(0x84);
        SU16(0x84, (xq & 0xFFFDu) | ((xq & 1u) << 1));
    }
    if (LS16(0xE4) != 0) {
        int32_t dv;
        SU16(0xE2, LU16(0xE2) + LU16(0xE4));
        k = func_8008E840(LU16(0x5A), (uint32_t)(LS16(0xE2) + LS16(0xDE)), LS16(0xE0)) << 16;
        dv = LU16(0x7E);
        SU16(0xE4, 0);
        SU16(0x7A, dv);
        if (dv == 0) {
            /* retail cc1 emits `break 7` (divide-by-zero trap) here. */
            fprintf(stderr, "FATAL: func_8008E8D0 portamento divide by zero (voice %08X)\n", v);
            abort();
        }
        SU32(0x4C, (int32_t)(k - ((uint32_t)AK_SHL(LI32(0x30), 16) + LU32(0x34))) / dv);
    }
    SU16(0x80, LU16(0xE2));
    SU16(0xE6, LU16(0xDE));
}

/* ── src/func_8008DB7C.c ─────────────────────────────────────────────── */
static uint32_t ak_tempo(pe_addr_t ctl)
{
    uint32_t t = PE_LoadU16(ctl + 0x22u);        /* TEMPO(c) */
    uint32_t m = PE_LoadU8(0x8009D2D2u);
    if (m != 0) {
        if (m < 0x80u)
            t += (t * m) >> 7;
        else
            t = (t * m) >> 8;
    }
    return t;
}

/* One sweep over a 24-voice sequence bank (both sequence loops). */
static void ak_bank_step(pe_addr_t bank, uint32_t act)
{
    uint32_t bit = 1;
    pe_addr_t p = bank;
    do {
        if (act & bit) {
            PE_StoreU16(p + 0x56u, (uint16_t)(PE_LoadU16(p + 0x56u) - 1u));
            PE_StoreU16(p + 0x58u, (uint16_t)(PE_LoadU16(p + 0x58u) - 1u));
            if (PE_LoadU16(p + 0x56u) == 0) {
                func_8008E8D0(p, bit);
            } else if (PE_LoadU16(p + 0x58u) == 0) {
                OR32(CTL + 0x18u, bit);
                AND32(CTL + 0x14u, ~bit);
            }
            func_80087AA8(p, bit);
            act ^= bit;
        }
        p += VOICE_SIZE;
        bit <<= 1;
    } while (act != 0);
}

void func_8008DB7C(void)
{
    uint32_t t, v, act, bit, w;
    pe_addr_t c, d, e, p;

    func_80089328();
    if (G32(CTL + 4u) != 0) {
        t = ak_tempo(CTL);
        c = CTL;
        v = G32(c + 0x28u) + t;
        S32(c + 0x28u, v);
        if ((v & 0xFFFF0000u) || (G32(0x8009D2DCu) & 4u)) {
            S32(c + 0x28u, v & 0xFFFFu);
            do {
                ak_bank_step(GA_BANK0, G32(CTL + 4u));
                c = CTL;
                if (PE_LoadU16(c + 0x52u) != 0) {
                    PE_StoreU16(c + 0x52u, (uint16_t)(PE_LoadU16(c + 0x52u) - 1u));
                    S32(c + 0x20u, G32(c + 0x20u) + G32(c + 0x24u));
                }
                c = CTL;
                if (PE_LoadU16(c + 0x58u) != 0) {
                    PE_StoreU16(c + 0x58u, (uint16_t)(PE_LoadU16(c + 0x58u) - 1u));
                    S32(c + 0x40u, G32(c + 0x40u) + G32(c + 0x44u));
                    OR32(0x8009D2C4u, 0x80u);
                }
                d = CTL;
                if (PE_LoadU16(d + 0x60u) != 0) {
                    uint16_t n62 = (uint16_t)(PE_LoadU16(d + 0x62u) + 1u);
                    PE_StoreU16(d + 0x62u, n62);
                    if (n62 == PE_LoadU16(d + 0x60u)) {
                        uint16_t n5e;
                        PE_StoreU16(d + 0x62u, 0);
                        n5e = (uint16_t)(PE_LoadU16(d + 0x5Eu) + 1u);
                        PE_StoreU16(d + 0x5Eu, n5e);
                        if (n5e == PE_LoadU16(d + 0x5Cu)) {
                            PE_StoreU16(d + 0x5Eu, 0);
                            PE_StoreU16(d + 0x64u, (uint16_t)(PE_LoadU16(d + 0x64u) + 1u));
                            if (G32(0x8009D22Cu) == 0)
                                break;
                            S32(0x8009D22Cu, G32(0x8009D22Cu) - 1u);
                        }
                    }
                }
            } while (G32(0x8009D22Cu) != 0);
        }
    }
    c = CTL;
    if (G32(c + CTL_SIZE + 4u) != 0) {
        t = PE_LoadU16(c + CTL_SIZE + 0x22u);
        S32(GA_D2C8, c + CTL_SIZE);
        {
            uint32_t m = PE_LoadU8(0x8009D2D2u);
            if (m != 0) {
                if (m < 0x80u)
                    t += (t * m) >> 7;
                else
                    t = (t * m) >> 8;
            }
        }
        e = CTL;
        w = G32(e + 0x28u) + t;
        S32(e + 0x28u, w);
        if ((w & 0xFFFF0000u) || (G32(0x8009D2DCu) & 4u)) {
            act = G32(e + 4u);
            S32(e + 0x28u, w & 0xFFFFu);
            ak_bank_step(GA_BANK1, act);
            c = CTL;
            if (PE_LoadU16(c + 0x52u) != 0) {
                PE_StoreU16(c + 0x52u, (uint16_t)(PE_LoadU16(c + 0x52u) - 1u));
                S32(c + 0x20u, G32(c + 0x20u) + G32(c + 0x24u));
            }
            c = CTL;
            if (PE_LoadU16(c + 0x58u) != 0) {
                PE_StoreU16(c + 0x58u, (uint16_t)(PE_LoadU16(c + 0x58u) - 1u));
                S32(c + 0x40u, G32(c + 0x40u) + G32(c + 0x44u));
            }
            d = CTL;
            if (PE_LoadU16(d + 0x60u) != 0) {
                uint16_t n62 = (uint16_t)(PE_LoadU16(d + 0x62u) + 1u);
                PE_StoreU16(d + 0x62u, n62);
                if (n62 == PE_LoadU16(d + 0x60u)) {
                    uint16_t n5e;
                    PE_StoreU16(d + 0x62u, 0);
                    n5e = (uint16_t)(PE_LoadU16(d + 0x5Eu) + 1u);
                    PE_StoreU16(d + 0x5Eu, n5e);
                    if (n5e == PE_LoadU16(d + 0x5Cu)) {
                        PE_StoreU16(d + 0x5Eu, 0);
                        PE_StoreU16(d + 0x64u, (uint16_t)(PE_LoadU16(d + 0x64u) + 1u));
                        if (G32(0x8009D22Cu) != 0)
                            S32(0x8009D22Cu, G32(0x8009D22Cu) - 1u);
                    }
                }
            }
        }
        S32(GA_D2C8, CTL - CTL_SIZE);
    }
    c = CTL;
    if (G32(c + 4u) == 0 && G32(c + 0x1Cu) == 0 && G32(c + CTL_SIZE + 4u) != 0) {
        func_8008D820(c + CTL_SIZE, c, CTL_SIZE);
        func_8008D820(GA_BANK1, GA_BANK1 - 24u * VOICE_SIZE, 0x1AA0u);
        PE_StoreU16(CTL + CTL_SIZE + 0x54u, 0);
        S32(CTL + CTL_SIZE + 4u, 0);
    }
    act = G32(0x800BCD50u);
    if (act != 0) {
        v = G32(0x800BCD68u) + PE_LoadU16(0x800BCD66u);
        S32(0x800BCD68u, v);
        if ((v & 0xFFFF0000u) || (G32(0x8009D2DCu) & 4u)) {
            S32(0x800BCD68u, v & 0xFFFFu);
            bit = 0x1000u;
            p = GA_BANKSFX;
            do {
                if (act & bit) {
                    if (!(G32(0x8009D2DCu) & 2u) || (G32(p + 0x2Cu) & 0x2000000u)) {
                        PE_StoreU16(p + 0x56u, (uint16_t)(PE_LoadU16(p + 0x56u) - 1u));
                        S32(p + 0x50u, G32(p + 0x50u) + 1u);
                        PE_StoreU16(p + 0x58u, (uint16_t)(PE_LoadU16(p + 0x58u) - 1u));
                        if (PE_LoadU16(p + 0x56u) == 0) {
                            func_8008E8D0(p, bit);
                        } else if (PE_LoadU16(p + 0x58u) == 0) {
                            OR32(0x800BCD5Cu, bit);
                            AND32(0x800BCD58u, ~bit);
                        }
                        func_80087FA0(p, bit);
                    }
                    act ^= bit;
                }
                p += VOICE_SIZE;
                bit <<= 1;
            } while (act != 0);
        }
    }
    if (G32(0x8009D268u) == 0)
        func_8008CA84();
    func_8008D844();
    func_80089784();
}

/* ── PE_AKAO_VERIFY: native vs retail differential ───────────────────────
 * Called from PE_Event_ServiceAudioCommands (pe_libetc.c) once per frame
 * wait when the retail RCNT2 event is armed and the audio lane's driver is
 * off (run with PE_AUDIO=0, so SPU registers are plain storage).
 *   PE_AKAO_VERIFY=1  snapshot guest RAM + SPU registers; run the retail
 *                     func_8008DB7C in the audio lane's interpreter
 *                     (PE_AudioDriver_CallGuest); restore; run the native
 *                     func_8008DB7C; compare; continue from the RETAIL result.
 *   PE_AKAO_VERIFY=2  native tick only (no interpreter).
 * 0x801FF000..0x801FFFFF (host scratch windows, retail stack) is excluded:
 * the interpreter runs on a private stack outside guest RAM. */
#include <string.h>
#include "pe_plat/audio.h"
extern uint32_t PE_AudioDriver_CallGuest(uint32_t entry, uint32_t a0, uint32_t a1,
                                         uint32_t a2, uint32_t a3, int *ok)
    __attribute__((weak));

int PE_AkaoVerify_Tick(void)
{
    static int mode = -1;
    static uint8_t *snap, *ra;
    static uint16_t spu_s[256], spu_a[256];
    static unsigned long ticks, bad, faults;
    static unsigned printed;
    uint8_t *ram;
    unsigned i, nd = 0;
    int ok = 1;

    if (mode < 0) {
        const char *e = getenv("PE_AKAO_VERIFY");
        mode = e ? atoi(e) : 0;
    }
    if (mode == 2) { func_8008DB7C(); pe_plat_audio_commit_registers(); return 1; }
    if (mode != 1) return 0;
    if (!PE_AudioDriver_CallGuest) {
        /* Native-only port binary: the retail-code oracle is linked into
         * test executables only. */
        static int said;
        if (!said) { said = 1; fprintf(stderr, "[AKAO_VERIFY] mode 1 needs the "
            "test-only interpreter oracle; not in this binary (native only)\n"); }
        mode = 0;
        return 0;
    }
    if (!snap) {
        snap = (uint8_t *)malloc(0x200000u);
        ra = (uint8_t *)malloc(0x200000u);
        if (!snap || !ra) abort();
    }
    pe_plat_audio_commit_registers();
    ram = (uint8_t *)PE_Translate(0x80000000u, 0x200000u);
    memcpy(snap, ram, 0x200000u);
    for (i = 0; i < 256u; i++) spu_s[i] = PE_SpuRegister_LoadU16(i * 2u);
    (void)PE_AudioDriver_CallGuest(0x8008DB7Cu, 0, 0, 0, 0, &ok);
    memcpy(ra, ram, 0x200000u);
    for (i = 0; i < 256u; i++) spu_a[i] = PE_SpuRegister_LoadU16(i * 2u);
    memcpy(ram, snap, 0x200000u);
    for (i = 0; i < 256u; i++) PE_SpuRegister_StoreU16(i * 2u, spu_s[i]);
    func_8008DB7C();
    pe_plat_audio_commit_registers();   /* flush host-pointer SPU writes */
    ticks++;
    if (!ok) {
        faults++;
        if (printed < 60u) { printed++; fprintf(stderr, "[AKAO_VERIFY] tick %lu: interpreter fault, native kept\n", ticks); }
        return 1;
    }
    for (i = 0; i < 0x1FF000u; i += 4u) {
        uint32_t a, b;
        memcpy(&a, ra + i, 4); memcpy(&b, ram + i, 4);
        if (a != b) {
            if (printed < 60u && nd < 6u) {
                printed++;
                fprintf(stderr, "[AKAO_VERIFY] tick %lu RAM %08X retail=%08X native=%08X\n",
                        ticks, 0x80000000u + i, a, b);
            }
            nd++;
        }
    }
    for (i = 0; i < 256u; i++) {
        uint16_t b = PE_SpuRegister_LoadU16(i * 2u);
        if (b != spu_a[i]) {
            if (printed < 60u && nd < 6u) {
                printed++;
                fprintf(stderr, "[AKAO_VERIFY] tick %lu SPU %03X retail=%04X native=%04X\n",
                        ticks, i * 2u, spu_a[i], b);
            }
            nd++;
        }
    }
    if (nd) bad++;
    if ((ticks % 2000u) == 0 || (nd && bad <= 3u))
        fprintf(stderr, "[AKAO_VERIFY] ticks=%lu mismatched=%lu faults=%lu | native calls: 8E8D0=%lu 87AA8=%lu 88980=%lu keyflags=%lu\n",
                ticks, bad, faults, g_ak_ne8d0, g_ak_n87aa8, g_ak_n88980, g_ak_nkon);
    /* continue from the retail result */
    memcpy(ram, ra, 0x200000u);
    for (i = 0; i < 256u; i++) PE_SpuRegister_StoreU16(i * 2u, spu_a[i]);
    return 1;
}
