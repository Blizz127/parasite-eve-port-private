/*
 * Hand adapters — port_absent lane round 4 (2026-09-24): func_80084B78,
 * func_80087FA0, func_8008D844, func_8008E2DC.  Bodies follow the matched
 * leaves src/func_XXXXXXXX.c; records are guest addresses read by offset,
 * guest pointers held in RAM are loaded with PE_LoadU32.  No retail stack
 * buffer of these leaves is handed to a guest-address callee, so
 * PE_HAND_R4_STACK (hand_absent_r4_protos.h) is reserved but unused.
 */
#include "pe_guest_decomp.h"
#include "game_port.h"   /* PE_Port_RequestStop */

void func_80089960(void);   /* generated TU (src/func_80089960.c) */
void func_80089CF0(void);   /* generated TU (src/func_80089CF0.c) */

/* ── src/func_80084B78.c: card record step dispatch.  If the first byte of
 * the command buffer (*(a0 + 0x3C)) is 0xF3 and a0[0xE8] == 0 -> tag 'C'
 * with 0 and return 0.  Otherwise by a0[0x46]: 1 -> 'C' with 1; 0xFE ->
 * 'C' with 0; 0 / 0xFF -> nothing; anything else -> the hook at a0 + 0x14
 * called with a0 (pe_dis.sh 0x80084B78 0xCC: `lw v0,20(a0)` / `jalr v0`
 * with $a0 = the record) when it is non-zero, else func_800835C0(a0).
 * The hook slot has no proven installed target: loud boundary carrying the
 * loaded address.  Returns 0. */
int func_80084B78(pe_addr_t a0)
{
    pe_addr_t hook;

    if (PE_LoadU8(PE_LoadU32(a0 + 0x3Cu)) == 0xF3u && PE_LoadU8(a0 + 0xE8u) == 0u) {
        func_80083E50(a0, 0);
        return 0;
    }
    switch (PE_LoadU8(a0 + 0x46u)) {
    case 1:
        func_80083E50(a0, 1);
        break;
    case 0xFE:
        func_80083E50(a0, 0);
        break;
    case 0:
    case 0xFF:
        break;
    default:
        hook = PE_LoadU32(a0 + 0x14u);
        if (hook != 0u)
            (void)PE_Decomp_Boundary("func_80084B78 a0->+0x14 card hook", hook, 1u,
                                     a0, 0u, 0u, 0u);
        else
            func_800835C0(a0);
        break;
    }
    return 0;
}

/* ── src/func_80087FA0.c: per-tick envelope/slide update of the 0x11C-byte
 * sequencer voice v (bit = the voice's mask bit).  Field widths and
 * signedness are the leaf's U16/S16/I32/U32 macros. */
static uint16_t r4_u16(pe_addr_t v, uint32_t o) { return PE_LoadU16(v + o); }
static int16_t r4_s16(pe_addr_t v, uint32_t o) { return (int16_t)PE_LoadU16(v + o); }
static int32_t r4_i32(pe_addr_t v, uint32_t o) { return (int32_t)PE_LoadU32(v + o); }
static void r4_or32(pe_addr_t v, uint32_t o, uint32_t m)
{
    PE_StoreU32(v + o, PE_LoadU32(v + o) | m);
}

/* `t = PTR(o); if (t[0] == 0 && t[1] == 0) t += t[2];` -> guest address of
 * the (possibly redirected) short table; returns t[0]. */
static int32_t r4_table_head(pe_addr_t v, uint32_t o)
{
    pe_addr_t t = PE_LoadU32(v + o);

    if (PE_LoadU16(t) == 0u && PE_LoadU16(t + 2u) == 0u)
        t += (uint32_t)((int32_t)(int16_t)PE_LoadU16(t + 4u) * 2);
    return (int16_t)PE_LoadU16(t);
}

void func_80087FA0(pe_addr_t v, unsigned int bit)
{
    int32_t n;
    uint32_t x;
    uint32_t m;
    int32_t a;

    if (r4_u16(v, 0x72) != 0) {
        PE_StoreU16(v + 0x72u, (uint16_t)(r4_u16(v, 0x72) - 1u));
        n = (int32_t)((uint32_t)r4_i32(v, 0x44) + (uint32_t)r4_i32(v, 0x48));
        if (((uint32_t)n & 0xFFE00000u) != ((uint32_t)r4_i32(v, 0x44) & 0xFFE00000u))
            r4_or32(v, 0xF4, 3u);
        PE_StoreU32(v + 0x44u, (uint32_t)n);
    }
    if (r4_u16(v, 0xBA) != 0) {
        PE_StoreU16(v + 0xBAu, (uint16_t)(r4_u16(v, 0xBA) - 1u));
        if (r4_u16(v, 0xBA) == 0) {
            PE_StoreU32(0x800BCD6Cu, PE_LoadU32(0x800BCD6Cu) ^ bit);
            PE_StoreU32(0x8009D2C4u, PE_LoadU32(0x8009D2C4u) | 0x10u);
            func_80089960();
        }
    }
    if (r4_u16(v, 0xBC) != 0) {
        PE_StoreU16(v + 0xBCu, (uint16_t)(r4_u16(v, 0xBC) - 1u));
        if (r4_u16(v, 0xBC) == 0) {
            PE_StoreU32(0x800BCD74u, PE_LoadU32(0x800BCD74u) ^ bit);
            func_80089CF0();
        }
    }
    if (r4_u16(v, 0x96) != 0) {
        PE_StoreU16(v + 0x96u, (uint16_t)(r4_u16(v, 0x96) - 1u));
        x = (uint32_t)r4_u16(v, 0x94) + r4_u16(v, 0x98);   /* untruncated */
        PE_StoreU16(v + 0x94u, (uint16_t)x);
        {
            uint32_t d = (x & 0x7F00u) >> 8;
            if (x & 0x8000u)
                m = d * PE_LoadU32(v + 0x30u);
            else
                m = d * ((PE_LoadU32(v + 0x30u) * 15u) >> 8);
        }
        PE_StoreU16(v + 0x92u, (uint16_t)(m >> 7));
        if (r4_u16(v, 0x8E) != 1) {
            int32_t t0 = r4_table_head(v, 0x1C);
            n = ((int32_t)r4_u16(v, 0x92) * t0) >> 16;
            if (n != r4_s16(v, 0xE8)) {
                PE_StoreU16(v + 0xE8u, (uint16_t)n);
                r4_or32(v, 0xF4, 0x10u);
                if (n >= 0)
                    PE_StoreU16(v + 0xE8u, (uint16_t)(n * 2));
            }
        }
    }
    if (r4_u16(v, 0xA8) != 0) {
        PE_StoreU16(v + 0xA8u, (uint16_t)(r4_u16(v, 0xA8) - 1u));
        PE_StoreU16(v + 0xA6u, (uint16_t)(r4_u16(v, 0xA6) + r4_u16(v, 0xAA)));
        if (r4_u16(v, 0xA2) != 1) {
            int32_t t0 = r4_table_head(v, 0x20);
            a = ((int32_t)r4_s16(v, 0x46) * (int32_t)(r4_u16(v, 0x6C) >> 8)) >> 7;
            /* `(a * (U16(0xA6) >> 8)) << 9 >> 16`: int, 32-bit wrap then an
             * arithmetic shift (retail sll 9 / sra 16). */
            n = (int32_t)(((uint32_t)a * (uint32_t)(r4_u16(v, 0xA6) >> 8)) << 9) >> 16;
            n = (n * t0) >> 15;
            if (n != r4_s16(v, 0xEA)) {
                PE_StoreU16(v + 0xEAu, (uint16_t)n);
                r4_or32(v, 0xF4, 3u);
            }
        }
    }
    if (r4_u16(v, 0xB6) != 0) {
        PE_StoreU16(v + 0xB6u, (uint16_t)(r4_u16(v, 0xB6) - 1u));
        PE_StoreU16(v + 0xB4u, (uint16_t)(r4_u16(v, 0xB4) + r4_u16(v, 0xB8)));
        if (r4_u16(v, 0xB0) != 1) {
            int32_t t0 = r4_table_head(v, 0x24);
            n = ((int32_t)(r4_u16(v, 0xB4) >> 8) * t0) >> 15;
            if (n != r4_s16(v, 0xEC)) {
                PE_StoreU16(v + 0xECu, (uint16_t)n);
                r4_or32(v, 0xF4, 3u);
            }
        }
    }
    if (r4_u16(v, 0x7A) != 0) {
        PE_StoreU16(v + 0x7Au, (uint16_t)(r4_u16(v, 0x7A) - 1u));
        n = (int32_t)((uint32_t)r4_i32(v, 0x34) + (uint32_t)r4_i32(v, 0x4C));
        if (((uint32_t)n & 0xFFFF0000u) != ((uint32_t)r4_i32(v, 0x34) & 0xFFFF0000u))
            r4_or32(v, 0xF4, 0x10u);
        PE_StoreU32(v + 0x34u, (uint32_t)n);
    }
}

/* ── src/func_8008D844.c: sequencer fade tick (every 4th call).
 * Voice (0x11C bytes, D_800BC03C): +0 pitch (int), +4 pitch_step,
 * +0x34 cnt_pitch, +0x38 cnt_pan, +0x3A vol (u16), +0x3C cnt_vol,
 * +0x9C pan (short), +0x9E pan_step, +0xA0 vol_step, +0xB8 flags.
 * Seq (0x68 bytes, *D_8009D2C8 and the next one): +4 on, +0x48 vol,
 * +0x4C step, +0x50 cnt (short).  D_8009D2C8 is advanced by one Seq and
 * restored exactly as the leaf does (two guest stores). */
#define R4_SEQ_SIZE 0x68u
static void r4_seq_fade(pe_addr_t s, pe_addr_t voices)
{
    if (PE_LoadU32(s + 4u) != 0u && (int16_t)PE_LoadU16(s + 0x50u) != 0) {
        int32_t v;
        PE_StoreU16(s + 0x50u, (uint16_t)((int16_t)PE_LoadU16(s + 0x50u) - 1));
        v = (int32_t)(PE_LoadU32(s + 0x48u) + PE_LoadU32(s + 0x4Cu));
        if (((uint32_t)v & 0x7F0000u) != (PE_LoadU32(s + 0x48u) & 0x7F0000u))
            func_8008AB9C(voices);
        PE_StoreU32(s + 0x48u, (uint32_t)v);
    }
}

void func_8008D844(void)
{
    int32_t v;
    uint32_t mask;
    uint32_t bit;
    pe_addr_t v2;

    PE_StoreU16(0x8009CDECu, (uint16_t)(PE_LoadU16(0x8009CDECu) + 1u));
    if ((PE_LoadU16(0x8009CDECu) & 3u) != 0)
        return;
    if ((int16_t)PE_LoadU16(0x8009D2A2u) != 0) {
        PE_StoreU16(0x8009D2A2u, (uint16_t)((int16_t)PE_LoadU16(0x8009D2A2u) - 1));
        PE_StoreU32(0x8009D2B4u, PE_LoadU32(0x8009D2B4u) + PE_LoadU32(0x8009D284u));
        func_8008D7D0();
    }
    if ((int16_t)PE_LoadU16(0x8009D220u) != 0) {
        PE_StoreU16(0x8009D220u, (uint16_t)((int16_t)PE_LoadU16(0x8009D220u) - 1));
        PE_StoreU32(0x8009D2D0u, PE_LoadU32(0x8009D2D0u) + PE_LoadU32(0x8009D214u));
    }
    if ((int16_t)PE_LoadU16(0x8009D21Eu) != 0) {
        PE_StoreU16(0x8009D21Eu, (uint16_t)((int16_t)PE_LoadU16(0x8009D21Eu) - 1));
        v = (int32_t)(PE_LoadU32(0x8009D2CCu) + PE_LoadU32(0x8009D210u));
        if (((uint32_t)v & 0xFF0000u) != (PE_LoadU32(0x8009D2CCu) & 0xFF0000u)) {
            pe_addr_t q = 0x800B8BB4u;
            for (bit = 24; bit != 0; bit--) {
                PE_StoreU32(q, PE_LoadU32(q) | 0x10u);
                q += 0x11Cu;
            }
        }
        PE_StoreU32(0x8009D2CCu, (uint32_t)v);
    }
    r4_seq_fade(PE_LoadU32(0x8009D2C8u), 0x800B8AC0u);
    PE_StoreU32(0x8009D2C8u, PE_LoadU32(0x8009D2C8u) + R4_SEQ_SIZE);
    r4_seq_fade(PE_LoadU32(0x8009D2C8u), 0x800BA560u);
    PE_StoreU32(0x8009D2C8u, PE_LoadU32(0x8009D2C8u) - R4_SEQ_SIZE);

    mask = PE_LoadU32(0x800BCD50u);
    if (mask == 0u)
        return;
    bit = 0x1000u;
    v2 = 0x800BC03Cu;
    do {
        if (bit == 0u) {
            /* A mask bit below 0x1000 is never cleared: retail loops
             * forever here (bit shifted out, no memory access).  The host
             * cannot spin, so this is a run-control stop request. */
            PE_Port_RequestStop(PE_PORT_STOP_EXPLICIT);
            return;
        }
        if (mask & bit) {
            if (PE_LoadU16(v2 + 0x38u) != 0u) {
                int16_t pan = (int16_t)PE_LoadU16(v2 + 0x9Cu);
                PE_StoreU16(v2 + 0x38u, (uint16_t)(PE_LoadU16(v2 + 0x38u) - 1u));
                v = pan + (int16_t)PE_LoadU16(v2 + 0x9Eu);
                if (((uint32_t)v & 0xFF00u) != ((uint32_t)(int32_t)pan & 0xFF00u))
                    PE_StoreU32(v2 + 0xB8u, PE_LoadU32(v2 + 0xB8u) | 3u);
                PE_StoreU16(v2 + 0x9Cu, (uint16_t)v);
            }
            if (PE_LoadU16(v2 + 0x3Cu) != 0u) {
                uint16_t vol = PE_LoadU16(v2 + 0x3Au);
                PE_StoreU16(v2 + 0x3Cu, (uint16_t)(PE_LoadU16(v2 + 0x3Cu) - 1u));
                v = vol + (int16_t)PE_LoadU16(v2 + 0xA0u);
                if (((uint32_t)v & 0xFF00u) != (vol & 0xFF00u))
                    PE_StoreU32(v2 + 0xB8u, PE_LoadU32(v2 + 0xB8u) | 3u);
                PE_StoreU16(v2 + 0x3Au, (uint16_t)v);
            }
            if (PE_LoadU16(v2 + 0x34u) != 0u) {
                int32_t pitch = (int32_t)PE_LoadU32(v2 + 0u);
                PE_StoreU16(v2 + 0x34u, (uint16_t)(PE_LoadU16(v2 + 0x34u) - 1u));
                v = (int32_t)((uint32_t)pitch + PE_LoadU32(v2 + 4u));
                if (((uint32_t)v & 0xFF00u) != ((uint32_t)pitch & 0xFF00u))
                    PE_StoreU32(v2 + 0xB8u, PE_LoadU32(v2 + 0xB8u) | 0x10u);
                PE_StoreU32(v2 + 0u, (uint32_t)v);
            }
            mask ^= bit;
        }
        bit <<= 1;
        v2 += 0x11Cu;
    } while (mask != 0u);
}

/* ── src/func_8008E2DC.c: peek the next sequence event of voice record a0.
 * p = *a0 (guest cursor), k = u16 a0+0xCE (loop-stack depth).  Opcode
 * bytes < 0x9A are notes: 0x8F..0x99 also clear +0x82 and bits 0/2 of
 * +0x84; the byte is returned.  0x9A..0x9F -> 0xA0.  D_8009B7BC[c] (and
 * D_8009B8BC[sub] after 0xFC) is a skip length; the rest is the leaf's
 * control flow, kept label for label. */
int func_8008E2DC(pe_addr_t a0)
{
    pe_addr_t p;
    unsigned int k;
    int c;
    int off;
    int lo;
    int hi;

    p = PE_LoadU32(a0);
    k = PE_LoadU16(a0 + 0xCEu);
top:
    c = PE_LoadU8(p);
redo:
    if ((unsigned int)c < 0x9A) {
        if ((unsigned int)c >= 0x8F) {
            PE_StoreU16(a0 + 0x82u, 0u);
            PE_StoreU16(a0 + 0x84u, (uint16_t)(PE_LoadU16(a0 + 0x84u) & 0xFFFAu));
        }
        return PE_LoadU8(p);
    }
    if ((unsigned int)c < 0xA0)
        return 0xA0;
    if (PE_LoadU8(0x8009B7BCu + (uint32_t)c) != 0) {
        off = PE_LoadU8(0x8009B7BCu + (uint32_t)c);
        goto skip;
    }
    switch (c) {
    case 0xFC:
        p++;
        c = PE_LoadU8(p);
        if (PE_LoadU8(0x8009B8BCu + (uint32_t)c) != 0) {
            off = PE_LoadU8(0x8009B8BCu + (uint32_t)c);
            goto skip;
        }
        if (c == 7)
            goto op7;
        if (c < 8) {
            if (c == 6)
                goto op6;
            goto redo;
        }
        if (c >= 10)
            goto top;
        p++;
        if ((int)PE_LoadU8(p) == (int)PE_LoadU16(a0 + 0x62u + k * 2u) + 1) {
            p++;
            lo = PE_LoadU8(p);
            p++;
            hi = PE_LoadU8(p);
            p++;
            k--;
            k &= 3;
            goto rel16tail;
        }
        p += 3;
        goto top;
    op6:
        p++;
        goto rel16;
    op7:
        p++;
        c = PE_LoadU8(p);
        p++;
        if ((unsigned int)PE_LoadU16(PE_LoadU32(0x8009D2C8u) + 0x56u) < (unsigned int)c)
            goto skip2;
    rel16:
        lo = PE_LoadU8(p);
        p++;
        hi = PE_LoadU8(p);
        p++;
    rel16tail:
        lo += hi << 8;
        off = (short)lo;
    skip:
        p += (uint32_t)off;
        goto top;
    skip2:
        p += 2;
        goto top;
    case 0xC9:
        p++;
        if ((int)PE_LoadU8(p) == (int)PE_LoadU16(a0 + 0x62u + k * 2u) + 1) {
            p++;
            k--;
            k &= 3;
            goto top;
        }
        goto jumpk;
    case 0xCB:
    case 0xCD:
    case 0xD1:
    case 0xDB:
        p++;
        PE_StoreU16(a0 + 0x82u, 0u);
        PE_StoreU16(a0 + 0x84u, (uint16_t)(PE_LoadU16(a0 + 0x84u) & 0xFFFAu));
        goto top;
    case 0xCC:
    case 0xD0:
        PE_StoreU16(a0 + 0x84u, (uint16_t)(PE_LoadU16(a0 + 0x84u) & 0xFFFAu));
        return 0xA0;
    case 0xCA:
        if (PE_LoadU32(a0 + 0x38u) & 0x200000u)
            goto dflt;
    jumpk:
        p = PE_LoadU32(a0 + 4u + k * 4u);
        goto top;
    default:
    dflt:
        PE_StoreU16(a0 + 0x82u, 0u);
        PE_StoreU16(a0 + 0x84u, (uint16_t)(PE_LoadU16(a0 + 0x84u) & 0xFFFAu));
        return 0xA0;
    }
}
