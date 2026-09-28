/*
 * Hand adapters — port lane round 7 (port2, 2026-09-27).  Each function is a
 * line-for-line translation of the matched src/func_X.c named in its comment.
 * Guest objects are 32-bit guest addresses.  Every field access is a
 * PE_Load/PE_Store at the leaf's own offset and width.  A retail pointer held
 * in RAM is re-loaded wherever the leaf re-loads it.  An indirect call goes
 * through PE_GuestCall with the retail code address.  Tests:
 * pc_port/tests/test_port7.h.
 */
#include "pe_guest_decomp.h"
#include <stdlib.h>

/* Callees with no canonical header prototype (generated TUs; signatures
 * copied from those definitions). */
unsigned int func_80089FE0(pe_addr_t pe_a0, unsigned int a1);
void func_80089960(void);
void func_80089B28(void);
void func_80089CF0(void);
unsigned char func_8007FC18(void);
int func_80080AE4(pe_addr_t pe_buffer, int value);
extern int func_80077AA4(int a0, int a1);
extern int func_80077CF4(int a0);

/* ── src/func_80068710.c ────────────────────────────────────────────────
 * Grid cursor step.  `it` is an array of o->n (u16 at o+0x26) packed cells
 * (x = e >> 22, y = (e >> 12) & 0x3FF).  wrap: jump to the bottom-right
 * (dir == 0) or top-left cell.  Otherwise step x by +/-16 on the current row
 * (D_8009CDC0/C4), fall back to the nearest cell on the row, then to the next
 * row (y +/- 16), then recurse with wrap.  The chosen index is latched in
 * D_8009CDBC (previous one in D_8009CDB8).  Rejected by the generator only
 * for its recursive self-call with pointer parameters. */
#define M10_EX(e) ((e) >> 22)
#define M10_EY(e) (((e) >> 12) & 0x3FF)
#define M10_IT(i) PE_LoadU32(it + 4u * (uint32_t)(i))
#define M10_N     ((int)PE_LoadU16(o + 0x26u))
#define M10_DB8   PE_DECOMP_SCALAR(0x8009CDB8u, int)
#define M10_DBC   PE_DECOMP_SCALAR(0x8009CDBCu, int)
#define M10_DC0   PE_DECOMP_SCALAR(0x8009CDC0u, int)
#define M10_DC4   PE_DECOMP_SCALAR(0x8009CDC4u, int)

int func_80068710(pe_addr_t o, pe_addr_t it, unsigned char dir, unsigned char wrap)
{
    int i;
    int bx;
    int best;
    int step;
    unsigned char found;
    unsigned char more;
    int x;
    int by;
    int prev;
    int sel;

    bx = 0x7FFF;
    best = 0;
    sel = 0;
    if (wrap) {
        by = 0x7FFF;
        if (!dir) {
            by = -1;
            bx = -1;
            for (i = 0; i < M10_N; i++) {
                if (by < (int)M10_EY(M10_IT(i)))
                    by = (int)M10_EY(M10_IT(i));
            }
            for (i = 0; i < M10_N; i++) {
                if (M10_EY(M10_IT(i)) == (uint32_t)by) {
                    if (bx < (int)M10_EX(M10_IT(i))) {
                        bx = (int)M10_EX(M10_IT(i));
                        sel = i;
                    }
                }
            }
            prev = M10_DBC;
            M10_DBC = sel;
            M10_DB8 = prev;
            M10_DC0 = (int)M10_EX(M10_IT(sel));
            M10_DC4 = (int)M10_EY(M10_IT(sel));
            return sel;
        } else {
            for (i = 0; i < M10_N; i++) {
                if ((int)M10_EY(M10_IT(i)) < by)
                    by = (int)M10_EY(M10_IT(i));
            }
            for (i = 0; i < M10_N; i++) {
                if (M10_EY(M10_IT(i)) == (uint32_t)by) {
                    if ((int)M10_EX(M10_IT(i)) < bx) {
                        bx = (int)M10_EX(M10_IT(i));
                        best = i;
                    }
                }
            }
        }
        prev = M10_DBC;
        M10_DBC = best;
        M10_DB8 = prev;
        M10_DC0 = (int)M10_EX(M10_IT(best));
        M10_DC4 = (int)M10_EY(M10_IT(best));
        return best;
    }
    step = 16;
    if (!dir)
        step = -16;
    found = 0;
    more = 0;
    for (i = 0; i < M10_N; i++) {
        x = (int)M10_EX(M10_IT(i));
        if (x == M10_DC0 + step) {
            if (M10_EY(M10_IT(i)) == (uint32_t)M10_DC4) {
                prev = M10_DBC;
                M10_DBC = i;
                M10_DB8 = prev;
                M10_DC0 = (int)M10_EX(M10_IT(i));
                M10_DC4 = (int)M10_EY(M10_IT(i));
                return i;
            }
        } else if (M10_EY(M10_IT(i)) == (uint32_t)M10_DC4 && M10_DC0 < x) {
            more = 1;
        }
    }
    if (!found && more == 1) {
        if (!dir) {
            bx = 0;
            for (i = 0; i < M10_N; i++) {
                if (M10_EY(M10_IT(i)) == (uint32_t)M10_DC4 &&
                    (int)M10_EX(M10_IT(i)) < M10_DC0 && bx < (int)M10_EX(M10_IT(i))) {
                    bx = (int)M10_EX(M10_IT(i));
                    best = i;
                    found = 1;
                }
            }
        } else {
            bx = 0x7FFF;
            for (i = 0; i < M10_N; i++) {
                if (M10_EY(M10_IT(i)) == (uint32_t)M10_DC4 &&
                    M10_DC0 < (int)M10_EX(M10_IT(i)) && (int)M10_EX(M10_IT(i)) < bx) {
                    bx = (int)M10_EX(M10_IT(i));
                    best = i;
                    found = 1;
                }
            }
        }
    }
    if (found == 1)
        goto done;
    found = 0;
    if (!dir) {
        bx = -1;
        for (i = 0; i < M10_N; i++) {
            if (M10_EY(M10_IT(i)) == (uint32_t)(M10_DC4 + step) &&
                bx < (int)M10_EX(M10_IT(i))) {
                bx = (int)M10_EX(M10_IT(i));
                best = i;
                found = 1;
            }
        }
    } else {
        bx = 0x7FFF;
        for (i = 0; i < M10_N; i++) {
            if (M10_EY(M10_IT(i)) == (uint32_t)(M10_DC4 + step) &&
                (int)M10_EX(M10_IT(i)) < bx) {
                bx = (int)M10_EX(M10_IT(i));
                best = i;
                found = 1;
            }
        }
    }
    if (!found)
        return func_80068710(o, it, dir, 1);
done:
    prev = M10_DBC;
    M10_DBC = best;
    M10_DB8 = prev;
    M10_DC0 = (int)M10_EX(M10_IT(best));
    M10_DC4 = (int)M10_EY(M10_IT(best));
    return best;
}

/* Pad-record byte fields (src/func_80083644.c / src/func_80084C4C.c). */
#define PR_B(a, o)     PE_LoadU8((a) + (o))
#define PR_SB(a, o, v) PE_StoreU8((a) + (o), (uint8_t)(v))
#define PR_RX(a)       PE_LoadU32((a) + 0x3Cu)   /* receive buffer pointer  */
#define PR_TX(a)       PE_LoadU32((a) + 0x30u)   /* transmit buffer pointer */

/* ── src/func_80083644.c ────────────────────────────────────────────────
 * Default mode handler, dispatched on the record phase a0[0x46].  2: latch
 * the reply header bytes rx[3..6] into E3/E4/E9/EA and clear E6/EC.  3: E6 =
 * rx[5] + (rx[4] << 8), clear the block counter 0x47.  4: count a block, EC
 * += 8 + ((rx[4] + 3) & 0x1FC); after EA blocks either fail (func_80083790 >
 * 0x80: phase 0xFE, error 2) or finish through func_800837C8(a0, a0 + 0x63)
 * and restart at phase 2.  Returns 0 while the phase is consumed, 1 for any
 * other phase. */
int func_80083644(pe_addr_t a0)
{
    int c;

    switch (PR_B(a0, 0x46u)) {
    case 2:
        PR_SB(a0, 0xE3u, PE_LoadU8(PR_RX(a0) + 3u));
        PR_SB(a0, 0xE4u, PE_LoadU8(PR_RX(a0) + 4u));
        PE_StoreU16(a0 + 0xE6u, 0u);
        PR_SB(a0, 0xE9u, PE_LoadU8(PR_RX(a0) + 5u));
        PR_SB(a0, 0xEAu, PE_LoadU8(PR_RX(a0) + 6u));
        PE_StoreU32(a0 + 0xECu, 0u);
        break;
    case 3: {
        pe_addr_t p = PR_RX(a0);
        int hi = PE_LoadU8(p + 4u);
        int lo = PE_LoadU8(p + 5u);

        PR_SB(a0, 0x47u, 0u);
        PE_StoreU16(a0 + 0xE6u, (uint16_t)(lo + (hi << 8)));
        break;
    }
    case 4: {
        int ee = (int)PE_LoadU32(a0 + 0xECu);
        int cc = PR_B(a0, 0x47u) + 1;
        int bb = PE_LoadU8(PR_RX(a0) + 4u);

        PR_SB(a0, 0x47u, cc);
        PE_StoreU32(a0 + 0xECu, (uint32_t)(ee + 8 + ((bb + 3) & 0x1FC)));
        c = cc;
        if ((unsigned char)c < PR_B(a0, 0xEAu))
            return 0;
        if (func_80083790(a0) > 0x80) {
            PR_SB(a0, 0x46u, 0xFEu);
            PR_SB(a0, 0x49u, 2u);
        } else {
            PR_SB(a0, 0x46u, 0xFFu);
            func_800837C8(a0, (int)(a0 + 0x63u));
            PR_SB(a0, 0x46u, 2u);
        }
        return 0;
    }
    default:
        return 1;
    }
    return 1;
}

/* ── src/func_80084C4C.c ────────────────────────────────────────────────
 * Receive hook (D_8009B744).  E8 = rx[0] >> 4 unless that is 0xF; otherwise
 * copy rx[0..n) into tx with tx[0] = 0 (n = a0[0x44]).  Notify D_8009B728(a0)
 * on a status change (the goto structure is the leaf's).  In phases 2..0xFD
 * a non-0xF3 reply notifies again.  Phases 0/1/0xFE/0xFF advance inline; any
 * other phase (when a0[0x36] is set) runs the record's mode handler at +0x18,
 * or func_80083644 when that is 0, and adds its result to the phase. */
static void pr_notify(pe_addr_t a0)
{
    (void)PE_GuestCall("@80084C4C:D_8009B728", PE_LoadU32(0x8009B728u), 1u,
                       (uintptr_t)a0, 0, 0, 0);
}

void func_80084C4C(pe_addr_t a0)
{
    int old;
    int v;
    int i;
    int s;
    pe_addr_t f;
    int r;

    old = PR_B(a0, 0xE8u);
    v = PE_LoadU8(PR_RX(a0)) >> 4;
    PR_SB(a0, 0xE8u, v);
    if (v == 0xF) {
        PR_SB(a0, 0xE8u, old);
    } else {
        PE_StoreU8(PR_TX(a0), 0u);
        PE_StoreU8(PR_TX(a0) + 1u, PE_LoadU8(PR_RX(a0)));
        i = 2;
        if (i < PR_B(a0, 0x44u)) {
            do {
                PE_StoreU8(PR_TX(a0) + (uint32_t)i, PE_LoadU8(PR_RX(a0) + (uint32_t)i));
                i++;
            } while (i < PR_B(a0, 0x44u));
        }
    }
    if (PE_LoadU8(PR_RX(a0) + 1u) == 0) {
        if (PR_B(a0, 0x46u) == 1) {
            if (PE_LoadU32(a0 + 0x14u) == 0u)
                goto check;
        }
        if (PR_B(a0, 0x50u) == 0)
            goto notify;
    }
check:
    if (func_80084F8C(a0) == 0) {
        if (PR_B(a0, 0x37u) == 0) {
            if (PR_B(a0, 0x4Au) == 0) {
                if (PR_B(a0, 0xE8u) != old) {
notify:
                    pr_notify(a0);
                }
            }
        }
    }
    PR_SB(a0, 0x4Au, 0u);
    if ((unsigned int)((PR_B(a0, 0x46u) - 2) & 0xFF) < 0xFCu) {
        if (PE_LoadU8(PR_RX(a0)) != 0xF3u)
            pr_notify(a0);
    }
    s = PR_B(a0, 0x46u);
    if (s != 0) {
        if (s != 0xFF) {
            if (PR_B(a0, 0x36u) == 0)
                return;
        }
    }
    switch (s) {
    case 0:
        if (PR_B(a0, 0xE8u) == 0)
            return;
        v = PR_B(a0, 0x46u);
        PR_SB(a0, 0x49u, 1u);
        goto bump;
    case 1:
        v = PR_B(a0, 0x46u);
        PR_SB(a0, 0x47u, 0u);
bump:
        PR_SB(a0, 0x46u, v + 1);
        return;
    case 0xFE:
        PR_SB(a0, 0x46u, 0xFFu);
        return;
    case 0xFF:
        return;
    }
    f = PE_LoadU32(a0 + 0x18u);
    if (f != 0u)
        r = PE_GuestCall("@80084C4C:mode", f, 1u, (uintptr_t)a0, 0, 0, 0);
    else
        r = func_80083644(a0);
    PR_SB(a0, 0x46u, PR_B(a0, 0x46u) + r);
}

/* ── polygon edge helpers (src/func_8001CBA0.c, src/func_8001CE88.c) ────
 * Pt is { short f0, x, f4, z } (stride 8); Vec is three ints.  Retail
 * arithmetic wraps (MIPS mult/addu), so products and shifts are done in
 * 32-bit unsigned and read back signed.  ASPSX's inline DIV guards execute
 * `break 7` on a zero divisor and `break 6` on INT_MIN / -1; the port aborts
 * there as the other hand ports do. */
#define P7_PT_X(p, k) ((int)(short)PE_LoadU16((p) + 8u * (uint32_t)(k) + 2u))
#define P7_PT_Z(p, k) ((int)(short)PE_LoadU16((p) + 8u * (uint32_t)(k) + 6u))
#define P7_CE2C       ((int)PE_LoadU16(0x8009CE2Cu))
#define P7_MUL(a, b)  ((int)((uint32_t)(a) * (uint32_t)(b)))
#define P7_SHL(a, n)  ((int)((uint32_t)(a) << (n)))

static int p7_div(int num, int den)
{
    if (den == 0 || (den == -1 && num == INT32_MIN)) {
        fprintf(stderr, "p7_div: retail DIV break (num=%d den=%d)\n", num, den);
        abort();
    }
    return num / den;
}

static void p7_vec(pe_addr_t v, int x, int y, int z)
{
    PE_StoreU32(v, (uint32_t)x);
    PE_StoreU32(v + 4u, (uint32_t)y);
    PE_StoreU32(v + 8u, (uint32_t)z);
}

#define P7_VX(v) ((int)PE_LoadU32((v)))
#define P7_VZ(v) ((int)PE_LoadU32((v) + 8u))

/* src/func_8001CBA0.c: edge (p[i-1] or p[n-1]) -> p[i].  b = unit edge
 * (func_80078134), project the object's step (f28/f30 relative to f40/f48)
 * onto it, then walk outward in 1.0 steps (+x, -x, +z, -z) until the
 * perpendicular distance exceeds D_8009CE2C and store that point. */
void func_8001CBA0(pe_addr_t o, pe_addr_t p, unsigned short n, short i)
{
    const pe_addr_t a = PE_HAND_P7_STACK, b = PE_HAND_P7_STACK + 0x10u;
    int x1, z1, x0, z0, d, px, pz, len, x, z, k, j, ax, az, bx, bz;

    x1 = P7_PT_X(p, i);
    z1 = P7_PT_Z(p, i);
    if (i > 0) {
        x0 = P7_PT_X(p, i - 1);
        z0 = P7_PT_Z(p, i - 1);
    } else {
        x0 = P7_PT_X(p, (int)n - 1);
        z0 = P7_PT_Z(p, (int)n - 1);
    }
    p7_vec(a, x1 - x0, 0, z1 - z0);
    (void)func_80078134(a, b);
    bx = P7_SHL(P7_VX(b), 4);
    bz = P7_SHL(P7_VZ(b), 4);
    PE_StoreU32(b, (uint32_t)bx);
    PE_StoreU32(b + 8u, (uint32_t)bz);
    px = (int)PE_LoadU32(o + 0x40u);
    pz = (int)PE_LoadU32(o + 0x48u);
    d = (int)func_8003708C((uint32_t)bx, PE_LoadU32(o + 0x28u) - (uint32_t)px);
    d = (int)((uint32_t)d + func_8003708C((uint32_t)bz, PE_LoadU32(o + 0x30u) - (uint32_t)pz));
    px = (int)func_8003708C((uint32_t)bx, (uint32_t)d);
    pz = (int)func_8003708C((uint32_t)bz, (uint32_t)d);
    px = (int)((uint32_t)px + PE_LoadU32(o + 0x40u));
    pz = (int)((uint32_t)pz + PE_LoadU32(o + 0x48u));
    ax = x0 - x1;
    az = z0 - z1;
    p7_vec(a, ax, 0, az);
    len = (int)func_80078004((uint32_t)P7_MUL(ax, ax) + (uint32_t)P7_MUL(az, az));
    d = p7_div((int)((uint32_t)P7_MUL((pz >> 16) - z1, ax) - (uint32_t)P7_MUL((px >> 16) - x1, az)), len);
    if (d < 0)
        d = -d;
    if (P7_CE2C < d) {
        PE_StoreU32(o + 0x28u, (uint32_t)px);
        PE_StoreU32(o + 0x30u, (uint32_t)pz);
        return;
    }
    for (k = 0;; k++) {
        for (j = 0; j < 4; j++) {
            x = px;
            z = pz;
            switch (j) {
            case 0: x = (int)((uint32_t)x + ((uint32_t)k << 16)); break;
            case 1: x = (int)((uint32_t)x - ((uint32_t)k << 16)); break;
            case 2: z = (int)((uint32_t)z + ((uint32_t)k << 16)); break;
            case 3: z = (int)((uint32_t)z - ((uint32_t)k << 16)); break;
            }
            d = p7_div((int)((uint32_t)P7_MUL((z >> 16) - z1, ax) - (uint32_t)P7_MUL((x >> 16) - x1, az)), len);
            if (d < 0)
                d = -d;
            if (P7_CE2C < d) {
                PE_StoreU32(o + 0x28u, (uint32_t)x);
                PE_StoreU32(o + 0x30u, (uint32_t)z);
                return;
            }
        }
    }
}

/* src/func_8001CE88.c: scan edges p[i-1] -> p[i] (starting from p[n-1]).
 * Skip edges whose bounding box misses the D_8009CE2C square around (x, z)
 * and edges farther than D_8009CE2C.  An edge counts when the projection of
 * (x, z) falls on it, or the point is within D_8009CE2C of an endpoint.
 * Returns its index, or -1. */
int func_8001CE88(short x, short z, pe_addr_t p, unsigned short n)
{
    const pe_addr_t a = PE_HAND_P7_STACK;
    unsigned short i;
    short x0, z0, ox, oz, x1, z1;
    int dx, dz, len, d, ax, az;

    x0 = (short)P7_PT_X(p, (int)n - 1);
    z0 = (short)P7_PT_Z(p, (int)n - 1);
    for (i = 0; i < n; i++) {
        ox = x0;
        oz = z0;
        x1 = x0 = (short)P7_PT_X(p, i);
        z1 = z0 = (short)P7_PT_Z(p, i);
        d = x - P7_CE2C;
        if (x1 < d && ox < d)
            continue;
        d = x + P7_CE2C;
        if (d < x1 && d < ox)
            continue;
        d = z - P7_CE2C;
        if (z1 < d && oz < d)
            continue;
        d = z + P7_CE2C;
        if (d < z1 && d < oz)
            continue;
        ax = ox - x1;
        az = oz - z1;
        p7_vec(a, ax, 0, az);
        len = (int)func_80078004((uint32_t)P7_MUL(ax, ax) + (uint32_t)P7_MUL(az, az));
        dz = z - z1;
        dx = x - x1;
        d = p7_div((int)((uint32_t)P7_MUL(dz, ax) - (uint32_t)P7_MUL(dx, az)), len);
        if (d < 0)
            d = -d;
        if (P7_CE2C < d)
            continue;
        (void)func_80078134(a, a);
        ax = P7_VX(a);
        az = P7_VZ(a);
        d = (int)func_8003708C((uint32_t)P7_SHL(ax, 4), (uint32_t)P7_SHL(dx, 16));
        d = (int)((uint32_t)d + func_8003708C((uint32_t)P7_SHL(az, 4), (uint32_t)P7_SHL(dz, 16)));
        if (d < 0) {
            d = (int)((uint32_t)P7_MUL(dx, dx) + (uint32_t)P7_MUL(dz, dz));
            if (P7_MUL(P7_CE2C, P7_CE2C) < d)
                continue;
        }
        if (len < (d >> 16)) {
            d = (int)((uint32_t)P7_MUL(x - ox, x - ox) + (uint32_t)P7_MUL(z - oz, z - oz));
            if (P7_MUL(P7_CE2C, P7_CE2C) < d)
                continue;
        }
        return i;
    }
    return -1;
}

/* ── src/func_8008A068.c ────────────────────────────────────────────────
 * Sequence start.  D_8009D2C8 (the sequence control block) takes the header
 * a0 at +0x2C.  Channel mask = a0[0] & 0xFFFFFF.  D_800BCD5C gains the voices
 * that func_80089FE0(D_800BA560, ctl+0x6C) does not report and that
 * D_800BCD50 does not hold.  The mask goes to +0x1C (D_8009D2DC bit 0) or +4;
 * +8/+0xC take the next two 24-bit header words; +0 bit 8 is copied from
 * D_8009CDE8 (bit 0 cleared).  Then the 24 channel records D_800B8AC0
 * (stride 0x11C): a masked channel takes its track pointer (u16 offset after
 * the header cursor) and the default field set, then func_8008F178(v, 0).
 * An unmasked channel gets the idle track D_8009B8F4, +0xF4 |= 0x4400 and
 * +0x116 = 5.  Ends by resetting the control block and running
 * func_80089960 / 80089B28 / 80089CF0. */
#define P7_W(p, o, v) PE_StoreU32((p) + (o), (uint32_t)(v))
#define P7_H(p, o, v) PE_StoreU16((p) + (o), (uint16_t)(v))

void func_8008A068(pe_addr_t a0)
{
    pe_addr_t q, c, v;
    uint32_t mask, rem, bit, r, f, t;

    q = a0;
    P7_W(PE_LoadU32(0x8009D2C8u), 0x2Cu, a0);
    mask = PE_LoadU32(a0) & 0xFFFFFFu;
    r = func_80089FE0(0x800BA560u, PE_LoadU32(PE_LoadU32(0x8009D2C8u) + 0x6Cu));
    t = ~PE_LoadU32(0x800BCD50u) & 0xFFFFFFu;
    PE_StoreU32(0x800BCD5Cu, PE_LoadU32(0x800BCD5Cu) | (~r & t));
    c = PE_LoadU32(0x8009D2C8u);
    f = PE_LoadU32(0x8009D2DCu) & 1u;
    P7_W(c, 0x18u, 0u);
    if (f) {
        P7_W(c, 0x4u, 0u);
        P7_W(c, 0x1Cu, PE_LoadU32(c + 0x1Cu) | mask);
    } else {
        P7_W(c, 0x1Cu, 0u);
        P7_W(c, 0x4u, PE_LoadU32(c + 0x4u) | mask);
    }
    q += 4u;
    c = PE_LoadU32(0x8009D2C8u);
    P7_W(c, 0x8u, PE_LoadU32(q) & 0xFFFFFFu);
    q += 4u;
    P7_W(c, 0xCu, PE_LoadU32(q) & 0xFFFFFFu);
    q += 8u;
    P7_W(c, 0x0u, PE_LoadU32(c) & ~0x101u);
    P7_W(c, 0x0u, PE_LoadU32(c) | (PE_LoadU32(0x8009CDE8u) & 0x100u));
    bit = 1u;
    v = 0x800B8AC0u;
    rem = 0xFFFFFFu;
    do {
        if (mask & bit) {
            r = PE_LoadU16(q);
            q += 2u;
            P7_W(v, 0x0u, q + r);
            P7_W(v, 0xF0u, 0x18u);
            P7_H(v, 0x56u, 4u);
            P7_H(v, 0x58u, 2u);
            P7_H(v, 0x6Cu, 0x7F00u);
            P7_W(v, 0x44u, 0x3FFF0000u);
            P7_H(v, 0xD8u, 0x4000u);
            P7_H(v, 0xE0u, 0u);
            P7_H(v, 0xDEu, 0u);
            P7_H(v, 0x82u, 0u);
            P7_W(v, 0x34u, 0u);
            P7_H(v, 0xE4u, 0u);
            P7_H(v, 0x7Au, 0u);
            P7_H(v, 0xD2u, 0u);
            P7_H(v, 0xD0u, 0u);
            P7_H(v, 0x76u, 0x8000u);
            P7_H(v, 0x78u, 0u);
            P7_H(v, 0x82u, 0u);
            P7_H(v, 0x74u, 0u);
            P7_H(v, 0x72u, 0u);
            P7_W(v, 0x14u, a0);
            P7_H(v, 0x84u, 0u);
            P7_H(v, 0xECu, 0u);
            P7_W(v, 0x38u, 0u);
            P7_H(v, 0xCEu, 0u);
            P7_H(v, 0xB4u, 0u);
            P7_H(v, 0xA6u, 0u);
            P7_H(v, 0x94u, 0u);
            P7_H(v, 0xB6u, 0u);
            P7_H(v, 0xA8u, 0u);
            P7_H(v, 0x96u, 0u);
            P7_H(v, 0xBCu, 0u);
            P7_H(v, 0xBAu, 0u);
            func_8008F178(v, 0);
        } else {
            P7_H(v, 0x56u, 3u);
            P7_H(v, 0x58u, 1u);
            P7_W(v, 0x0u, 0x8009B8F4u);
            P7_W(v, 0xF4u, PE_LoadU32(v + 0xF4u) | 0x4400u);
            P7_H(v, 0x116u, 5u);
        }
        rem &= ~bit;
        mask &= ~bit;
        v += 0x11Cu;
        bit <<= 1;
    } while (rem != 0u);
    c = PE_LoadU32(0x8009D2C8u);
    P7_W(c, 0x20u, 0xFFFF0000u);
    P7_W(c, 0x28u, 1u);
    P7_H(c, 0x52u, 0u);
    P7_W(c, 0x40u, 0u);
    P7_H(c, 0x58u, 0u);
    P7_W(c, 0x44u, 0u);
    PE_StoreU32(0x8009D2C4u, 0u);
    P7_H(c, 0x62u, 0u);
    P7_H(c, 0x60u, 0u);
    P7_H(c, 0x5Eu, 0u);
    P7_H(c, 0x64u, 0u);
    P7_W(c, 0x34u, 0u);
    P7_W(c, 0x38u, 0u);
    P7_W(c, 0x3Cu, 0u);
    P7_H(c, 0x56u, 0u);
    P7_W(c, 0x10u, 0u);
    P7_W(c, 0x14u, 0xFFFFFFu);
    P7_W(c, 0x30u, 0u);
    func_80089960();
    func_80089B28();
    func_80089CF0();
}

/* ── src/func_80081E70.c ────────────────────────────────────────────────
 * DsRead ready callback over the read-control block D_8009B6EC { pos, last,
 * cb, mode, cnt, a, b, c, d }.  pos == -1 is seeded from the current
 * position (func_80080C48(func_8007FC28())).  DataReady (1): with a header
 * sector (status & 0x20) the 3-word header goes into buf (func_80080AE4 with
 * the ready callback masked).  A position that differs from pos is an error.
 * A newer one is reported as cb(1, result, buf).  Without a header, any
 * pos > last is reported.  pos then advances.  DiskError (4): cb(4, ...).
 * Otherwise a 0x10 status bit ends the read (restore callbacks a/b, d = 0,
 * cb(intr, ...)), unless c == 1.  A non-busy, non-seek/read status is an
 * error.  An error (mode == 1) retries through func_80082204 while cnt > 0
 * or == -1.  Once retries are spent it restores the callbacks (d == 1),
 * issues CdlPause, clears d and reports cb(5, ...).  The retail 16-byte stack
 * buffer lives at PE_HAND_P7_STACK + 0x20. */
#define DS_RCB 0x8009B6ECu
#define DS_F(o) PE_DECOMP_SCALAR(DS_RCB + (o), int)

static void ds_cb(int code, pe_addr_t result, pe_addr_t buf)
{
    (void)PE_GuestCall("@80081E70:cb", (pe_addr_t)DS_F(0x8u), 3u, (uintptr_t)(uint32_t)code,
                       (uintptr_t)result, (uintptr_t)buf, 0);
}

void func_80081E70(unsigned char intr, pe_addr_t result)
{
    const pe_addr_t buf = PE_HAND_P7_STACK + 0x20u;
    const int one = 1;
    int t, n, iv;

    if (DS_F(0x0u) == -1)
        DS_F(0x0u) = func_80080C48(func_8007FC28());
    iv = intr;
    if (iv == one) {
        if (func_8007FC18() & 0x20) {
            t = (int)func_800824F0(0u);
            (void)func_80080AE4(buf, 3);
            (void)func_800824F0((pe_addr_t)t);
            n = func_80080C48(buf);
            if (n != DS_F(0x0u))
                goto err;
            if (DS_F(0x8u) != 0 && DS_F(0x4u) < n) {
                ds_cb(1, result, buf);
                DS_F(0x4u) = n;
            }
        } else {
            if (DS_F(0x8u) != 0 && DS_F(0x0u) > DS_F(0x4u)) {
                ds_cb(1, result, buf);
                DS_F(0x4u) = DS_F(0x0u);
            }
        }
        DS_F(0x0u)++;
    } else if (iv == 4) {
        if (DS_F(0x8u) != 0)
            ds_cb(4, result, buf);
    } else {
        if (PE_LoadU8(result) & 0x10) {
            if (DS_F(0x1Cu) == one) {
                (void)func_800824C8(0);
                return;
            }
            (void)func_800824C8(DS_F(0x14u));
            (void)func_800824DC(DS_F(0x18u));
            DS_F(0x20u) = 0;
            if (DS_F(0x8u) != 0)
                ds_cb(iv, result, buf);
            return;
        }
        if (func_8007F778() == 0 && (PE_LoadU8(result) & 0xA0) == 0) {
err:
            DS_F(0xCu) = one;
        }
    }
    if (DS_F(0xCu) == 1) {
        if (DS_F(0x10u) > 0 || DS_F(0x10u) == -1) {
            func_80082204();
            if (DS_F(0x10u) > 0)
                DS_F(0x10u)--;
        } else {
            if (DS_F(0x20u) == 1) {
                (void)func_800824C8(DS_F(0x14u));
                (void)func_800824DC(DS_F(0x18u));
                (void)func_8007EE84(9u, 0u, 0u, 0xFFFFFFFFu);
            }
            DS_F(0x20u) = 0;
            if (DS_F(0x8u) != 0)
                ds_cb(5, result, buf);
        }
        DS_F(0xCu) = 0;
    }
}

/* ── src/func_8001D170.c ────────────────────────────────────────────────
 * Player/polygon boundary step over the actor D_8009D254 and the boundary
 * polygon D_8009D2F8 (D_8009D264 vertices).  The collision radius
 * D_8009CE2C (volatile: two stores) = (h26 * h224) / 4096, rounded toward
 * zero, after a first store of h224.  If the current position
 * (+0x2A, +0x32) hits an edge, func_8001CBA0 pushes the actor out.  Retail
 * passes two more stack arguments that the callee never reads.  If the
 * pushed position still hits an edge (low half of the result negative), the
 * step is cancelled: +0x28/+0x2C/+0x30 = +0x40/+0x44/+0x48. */
void func_8001D170(void)
{
    pe_addr_t s = PE_LoadU32(0x8009D254u), q;
    unsigned short h224 = PE_LoadU16(s + 0x224u), h26 = PE_LoadU16(s + 0x26u);
    int prod, h2A, h32, g1;
    unsigned int g2;
    short r;

    PE_StoreU16(0x8009CE2Cu, h224);
    prod = P7_MUL((int)h26, (int)h224);
    if (prod < 0)
        prod += 4095;
    h2A = (short)PE_LoadU16(s + 0x2Au);
    h32 = (short)PE_LoadU16(s + 0x32u);
    g1 = (int)PE_LoadU32(0x8009D2F8u);
    g2 = PE_LoadU16(0x8009D264u);
    PE_StoreU16(0x8009CE2Cu, (uint16_t)(prod >> 12));
    r = (short)func_8001CE88((short)h2A, (short)h32, (pe_addr_t)g1, (unsigned short)g2);
    if (r < 0)
        return;
    func_8001CBA0(PE_LoadU32(0x8009D254u), PE_LoadU32(0x8009D2F8u),
                  PE_LoadU16(0x8009D264u), r);
    s = PE_LoadU32(0x8009D254u);
    if ((int)((uint32_t)func_8001CE88((short)PE_LoadU16(s + 0x2Au), (short)PE_LoadU16(s + 0x32u),
                                      PE_LoadU32(0x8009D2F8u),
                                      PE_LoadU16(0x8009D264u)) << 16) < 0)
        return;
    q = PE_LoadU32(0x8009D254u);
    PE_StoreU32(q + 0x28u, PE_LoadU32(q + 0x40u));
    PE_StoreU32(q + 0x2Cu, PE_LoadU32(q + 0x44u));
    PE_StoreU32(q + 0x30u, PE_LoadU32(q + 0x48u));
}

/* ── src/func_800DE7A8.c ────────────────────────────────────────────────
 * Battle-overlay particle callback (pool of func_800DEA30).  Mode 1: move
 * by the velocity a1[3..5], apply gravity a1[4] += 1, damp x/z by 31/32.
 * Returns 1 (expire) once the timer D_800E27EC reaches 0x20.  Mode 2: draw
 * two billboards through func_800CEE20 at (a1[0..2]), angle a1[6] +
 * 8 * timer.  Brightness is a1[7] faded by timer/32.  The first sprite's
 * scale is func_80077CF4(timer << 5) + 0x2800, the second's 0x1800.  The
 * CLUT row comes from D_800E1204[D_800F336C] (+4 for slot 4 while
 * D_800F3428).  The retail stack vectors live at PE_HAND_P7_STACK + 0x30. */
int func_800DE7A8(int a0, pe_addr_t a1)
{
    const pe_addr_t pos = PE_HAND_P7_STACK + 0x30u, vec = PE_HAND_P7_STACK + 0x38u;
#define DE7_A(k) ((int)(short)PE_LoadU16(a1 + 2u * (k)))
#define DE7_SA(k, v) PE_StoreU16(a1 + 2u * (k), (uint16_t)(v))
    int s0v, s1v, w, idx, e, tm;

    tm = (int)PE_LoadU32(0x800E27ECu);
    switch (a0) {
    case 1:
        DE7_SA(0, DE7_A(0) + DE7_A(3));
        DE7_SA(1, DE7_A(1) + DE7_A(4));
        DE7_SA(2, DE7_A(2) + DE7_A(5));
        DE7_SA(4, DE7_A(4) + 1);
        DE7_SA(3, DE7_A(3) * 31 / 32);
        DE7_SA(5, DE7_A(5) * 31 / 32);
        if ((int)PE_LoadU32(0x800E27ECu) < 0x20)
            break;
        return 1;
    case 2:
        e = DE7_A(7);
        s1v = e - P7_MUL(e, tm) / 32;
        PE_StoreU16(pos, (uint16_t)DE7_A(0));
        PE_StoreU16(pos + 2u, (uint16_t)DE7_A(1));
        PE_StoreU16(pos + 4u, (uint16_t)DE7_A(2));
        PE_StoreU16(vec, 0u);
        PE_StoreU16(vec + 2u, 0u);
        PE_StoreU16(vec + 4u, (uint16_t)(DE7_A(6) + P7_SHL(tm, 3)));
        s0v = func_80077CF4(P7_SHL(tm, 5)) + 0x2800;
        idx = PE_LoadU16(0x800F336Cu);
        w = PE_LoadU16(0x800E1204u + 2u * (uint32_t)idx);
        if (idx == 4 && PE_LoadU32(0x800F3428u) != 0u)
            w += 4;
        func_800CEE20(pos, vec, s0v, s0v,
                      P7_MUL((short)PE_LoadU16(0x800F336Au), tm / 4) + 0xC0,
                      (uint32_t)func_80077AA4(0x40, w) & 0xFFFFu, 1, s1v, 0u);
        s0v = 0x1800;
        idx = PE_LoadU16(0x800F336Cu);
        w = PE_LoadU16(0x800E1204u + 2u * (uint32_t)idx);
        if (idx == 4 && PE_LoadU32(0x800F3428u) != 0u)
            w += 4;
        func_800CEE20(pos, vec, s0v, s0v,
                      P7_MUL((short)PE_LoadU16(0x800F336Au), (int)PE_LoadU32(0x800E27ECu) / 8) + 0x60,
                      (uint32_t)func_80077AA4(0, w) & 0xFFFFu, 3, s1v, 0u);
        break;
    default:
        return 0;
    }
    return 0;
#undef DE7_A
#undef DE7_SA
}

/* ── inventory item records (src/func_80058E44.c, src/func_80058FEC.c) ──
 * item(v): ids 0x100..0x17F -> D_800BEEAC + v*32, 1..0xFF ->
 * func_8005DB44(v - 1), 0x200..0x208 -> D_8009DE64 + v*32, else 0 (the
 * generator rejects the (v << 5) + (int)D_800BEEAC form).  A 0 record is
 * still dereferenced by retail at +6, which reads KUSEG low RAM. */
extern unsigned int func_80052F70(void);
extern pe_addr_t func_8005DB44(unsigned int index);
extern void func_80055760(void);
extern void func_8004F448(void);
extern int32_t func_8005415C(int32_t index);

static pe_addr_t inv_item(int v)
{
    if ((unsigned int)(v - 0x100) < 0x80u)
        return (pe_addr_t)((uint32_t)v << 5) + 0x800BEEACu;
    if ((unsigned int)(v - 1) < 0xFFu)
        return func_8005DB44((unsigned int)(v - 1));
    if ((unsigned int)(v - 0x200) < 9u)
        return (pe_addr_t)((uint32_t)v << 5) + 0x8009DE64u;
    return 0u;
}

#define INV_D048 PE_LoadU32(0x8009D048u)          /* short *list  */
#define INV_D07C PE_LoadU32(0x8009D07Cu)          /* short *equip */
#define INV_D050 ((int)PE_LoadU32(0x8009D050u))

/* src/func_80058E44.c.  1 when slot idx is out of range or empty.
 * Otherwise 0, or 1 when the record is not locked (+5 bit 5) and, in the
 * D_800C0E48 list, idx is neither D_800C0E20 nor D_800C0E22.  A kind-8
 * record (+6) also needs at least two kind-8 slots (func_8005415C). */
int func_80058E44(int idx)
{
    int s2 = 1, flag, i, count;
    pe_addr_t p;

    if (idx >= 0 && idx < INV_D050)
        p = inv_item((short)PE_LoadU16(INV_D048 + 2u * (uint32_t)idx));
    else
        p = 0u;
    if (p == 0u)
        return s2;
    s2 = 0;
    if ((PE_LoadU8(p + 5u) & 0x20u) == 0u) {
        flag = 0;
        if (INV_D048 == 0x800C0E48u) {
            if ((signed char)PE_LoadU8(0x800C0E20u) == idx ||
                (signed char)PE_LoadU8(0x800C0E22u) == idx)
                flag = 1;
        }
        if (flag == 0)
            s2 = 1;
    }
    if (PE_LoadU8(p + 6u) != 8u)
        return s2;
    i = 0;
    count = 0;
    if (INV_D050 > 0) {
        do {
            if (func_8005415C(i) == 8)
                count++;
            i++;
        } while (i < INV_D050);
    }
    if (count < 2)
        s2 = 0;
    else
        s2 &= 1;
    return s2;
}

/* The leaf's xor swap: p == q zeroes the entry, as retail does. */
static void inv_swap(pe_addr_t p, pe_addr_t q)
{
    PE_StoreU16(p, (uint16_t)(PE_LoadU16(p) ^ PE_LoadU16(q)));
    PE_StoreU16(q, (uint16_t)(PE_LoadU16(q) ^ PE_LoadU16(p)));
    PE_StoreU16(p, (uint16_t)(PE_LoadU16(p) ^ PE_LoadU16(q)));
}

static int inv_slot(int i)
{
    return (i >= 0 && i < (int)PE_LoadU32(0x8009D044u))
        ? (short)PE_LoadU16(0x800A1E00u + 2u * (uint32_t)i) : 0;
}

static void inv_fix_cursor(pe_addr_t g, int x, int y)
{
    if ((signed char)PE_LoadU8(g) == x)
        PE_StoreU8(g, (uint8_t)y);
    else if ((signed char)PE_LoadU8(g) == y)
        PE_StoreU8(g, (uint8_t)x);
}

/* src/func_80058FEC.c.  Selects the D_800C0E48 list (D_8009D050 =
 * func_80052F70(), D_8009D058 = D_8009D05C, D_8009D064 = 2), then swaps:
 * 0x34/0x34 two equipment entries.  0x33/0x33 two list slots, keeping the
 * cursors D_800C0E20/22 on their items.  0x34 with 0x33 (either order)
 * swaps an equipment entry with a list slot.  That swap needs
 * func_80058E44(slot) (retail's argument-less call leaves the slot in $a0,
 * pe_dis 0x80059220 / 0x80059388) and an equipment record whose kind (+6)
 * is outside 0x13..0x15; it then runs func_8004F448.  Every success runs
 * func_80055760 and returns 1; a refusal returns 0. */
int func_80058FEC(int a, int b, int c, int d)
{
    int x;

    PE_StoreU32(0x8009D048u, 0x800C0E48u);
    PE_StoreU32(0x8009D050u, func_80052F70());
    PE_StoreU32(0x8009D058u, 0x8009D05Cu);
    PE_StoreU32(0x8009D064u, 2u);
    if (a == 0x34 && c == a) {
        inv_swap(INV_D07C + 2u * (uint32_t)b, INV_D07C + 2u * (uint32_t)d);
        goto done;
    }
    if (a == 0x33 && c == a) {
        int xs = inv_slot(b), ys = inv_slot(d);
        inv_swap(INV_D048 + 2u * (uint32_t)xs, INV_D048 + 2u * (uint32_t)ys);
        inv_fix_cursor(0x800C0E20u, xs, ys);
        inv_fix_cursor(0x800C0E22u, xs, ys);
        goto done;
    }
    if (a == 0x34) {
        x = inv_slot(d);
        if (func_80058E44(x) == 0)
            return 0;
        if (PE_LoadU8(inv_item((short)PE_LoadU16(INV_D07C + 2u * (uint32_t)b)) + 6u) >= 0x13u &&
            PE_LoadU8(inv_item((short)PE_LoadU16(INV_D07C + 2u * (uint32_t)b)) + 6u) < 0x16u)
            return 0;
        inv_swap(INV_D07C + 2u * (uint32_t)b, INV_D048 + 2u * (uint32_t)x);
        func_8004F448();
    } else {
        x = inv_slot(b);
        if (func_80058E44(x) == 0)
            return 0;
        if (PE_LoadU8(inv_item((short)PE_LoadU16(INV_D07C + 2u * (uint32_t)d)) + 6u) >= 0x13u &&
            PE_LoadU8(inv_item((short)PE_LoadU16(INV_D07C + 2u * (uint32_t)d)) + 6u) < 0x16u)
            return 0;
        inv_swap(INV_D048 + 2u * (uint32_t)x, INV_D07C + 2u * (uint32_t)d);
        func_8004F448();
    }
done:
    func_80055760();
    return 1;
}

/* ── src/func_80054F58.c ────────────────────────────────────────────────
 * Two-sided exchange check.  p / q = records of this side's and the other
 * side's current list slot (func_80059F08(side), func_80059F08(!side)).  A
 * slot outside [0, D_8009D050) gives record 0, which retail still reads (KUSEG
 * low RAM).  Records carry a count at +0x14 and entries at +0x15.  With
 * k >= 0: 4 when the other side already holds entry p[k]; 3 when it has no
 * free (0) entry and no entry of p[k]'s class (& 0xE0) (or p[k] has none).
 * Then, in the D_800C0E48 list: if this side is the equipped D_800C0E22 slot,
 * the first entry of kind 8..10 decides r = func_80054E4C(1 << kind).  r == 0
 * becomes 5 unless it is entry k itself with k >= 0.  If the other side is the
 * equipped slot, m = (1 << its kind) - (1 << p[k]'s kind) goes to
 * func_80054E4C.  Otherwise 1. */
extern int32_t func_80059F08(uint32_t index);
extern int32_t func_80054E4C(int32_t extra);

static pe_addr_t inv_rec(int a0)
{
    if (a0 >= 0 && a0 < INV_D050)
        return inv_item((short)PE_LoadU16(INV_D048 + 2u * (uint32_t)a0));
    return 0u;
}

#define XC_N(r)    ((int)PE_LoadU8((r) + 0x14u))
#define XC_E(r, i) ((int)PE_LoadU8((r) + 0x15u + (uint32_t)(i)))

int func_80054F58(int side, int k)
{
    int s_me, s_other, r, m, i, a, d, x = 0, y = 0;
    pe_addr_t p, q;

    m = 0;
    s_me = func_80059F08((uint32_t)side);
    p = inv_rec(s_me);
    s_other = func_80059F08((uint32_t)(side == 0));
    q = inv_rec(s_other);
    r = 1;
    if (k >= 0) {
        a = XC_E(p, k);
        for (i = 0; i < XC_N(q); i++)
            if (XC_E(q, i) == a)
                break;
        if (i < XC_N(q))
            return 4;
        for (i = 0; i < XC_N(q); i++)
            if (XC_E(q, i) == 0)
                break;
        if (i >= XC_N(q)) {
            a = XC_E(p, k) & 0xE0;
            if (a == 0)
                return 3;
            for (i = 0; i < XC_N(q); i++)
                if ((XC_E(q, i) & 0xE0) == a)
                    break;
            if (i >= XC_N(q))
                return 3;
        }
    }
    (void)func_80059F08((uint32_t)side);
    if (INV_D048 == 0x800C0E48u && s_me == (signed char)PE_LoadU8(0x800C0E22u)) {
        for (i = 0; i < XC_N(p); i++) {
            x = (XC_E(p, i) & 0x1F) - 8;
            if ((unsigned int)x < 3u)
                break;
        }
        if (i < XC_N(p)) {
            d = i != k;
            r = func_80054E4C(1 << x);
            if (r == 0) {
                if (d)
                    goto five;
                if (k >= 0)
                    return r;
five:
                r = 5;
            }
        }
        return r;
    }
    (void)func_80059F08((uint32_t)(side == 0));
    if (INV_D048 != 0x800C0E48u)
        return 1;
    if (s_other != (signed char)PE_LoadU8(0x800C0E22u))
        return 1;
    if (k < 0)
        return 1;
    x = (XC_E(p, k) & 0x1F) - 8;
    if ((unsigned int)x >= 3u)
        return 1;
    for (i = 0; i < XC_N(q); i++) {
        y = (XC_E(q, i) & 0x1F) - 8;
        if ((unsigned int)y < 3u)
            break;
    }
    if (i < XC_N(q))
        m = (1 << y) - (1 << x);
    return func_80054E4C(m);
}

/* ── src/func_80088344.c ────────────────────────────────────────────────
 * Per-voice modulation tick over the 0x11C-byte channel record v (+0x38
 * flags).  Bit 0: pan LFO (table at +0x1C, period +0x8C/+0x8E, depth +0x92)
 * -> +0xE8, pitch dirty (+0xF4 |= 0x10).  Bit 1: volume LFO (+0x20) ->
 * +0xEA.  Bit 2: pan offset LFO (+0x24) -> +0xEC; both set volume dirty
 * (+0xF4 |= 3).  Bit 5: volume from the note header (v - 0x10).  A dirty
 * volume is recomputed into +0x118/+0x11A through the pan law D_8009B8F8 for
 * modes 1 and 4, where mode 4 inverts one side by the channel mask.  Other
 * modes use mono D_8009B9F8.  Bit 11 also writes the linked channel
 * (+0x5C) with a +0x5E reverb split.  Bit 4, or a dirty pitch, rebuilds
 * +0x10C from the base pitch and the D_8009D2CE transpose.  A table whose
 * next two entries are 0 loops back by its third entry.  Retail mult/sll
 * wrap, so those products are 32-bit here. */
#define VX_U16(o)   PE_LoadU16(v + (uint32_t)(o))
#define VX_S16(o)   ((int)(short)PE_LoadU16(v + (uint32_t)(o)))
#define VX_SET16(o, x) PE_StoreU16(v + (uint32_t)(o), (uint16_t)(x))
#define VX_U32(o)   PE_LoadU32(v + (uint32_t)(o))

/* *PTR(o)++ with the loop-back check done by the caller. */
static int vx_lfo_next(pe_addr_t v, uint32_t o)
{
    pe_addr_t t = VX_U32(o);
    int s = (short)PE_LoadU16(t);
    PE_StoreU32(v + o, t + 2u);
    return s;
}

static void vx_lfo_loop(pe_addr_t v, uint32_t o)
{
    pe_addr_t t = VX_U32(o);
    if (PE_LoadU16(t) == 0u && PE_LoadU16(t + 2u) == 0u)
        PE_StoreU32(v + o, t + 2u * (uint32_t)(int)(short)PE_LoadU16(t + 4u));
}

/* One pan-law evaluation into +0x118/+0x11A. */
static void vx_pan(pe_addr_t v, int vol, int pan, unsigned int mask)
{
    int r;

    switch ((int)PE_LoadU32(0x8009D2C0u)) {
    case 1:
        VX_SET16(0x118, P7_MUL(vol, (short)PE_LoadU16(0x8009B8F8u + 2u * (uint32_t)pan)) >> 15);
        VX_SET16(0x11A, P7_MUL(vol, (short)PE_LoadU16(0x8009B8F8u + 2u * (uint32_t)(pan ^ 0xFF))) >> 15);
        break;
    case 4:
        VX_SET16(0x118, P7_MUL(vol, (short)PE_LoadU16(0x8009B8F8u + 2u * (uint32_t)pan)) >> 15);
        r = P7_MUL(vol, (short)PE_LoadU16(0x8009B8F8u + 2u * (uint32_t)(pan ^ 0xFF))) >> 15;
        VX_SET16(0x11A, r);
        if (mask & 0xAAAAAAu)
            VX_SET16(0x11A, ~r);
        else
            VX_SET16(0x118, ~(int)VX_U16(0x118));
        break;
    default:
        VX_SET16(0x11A, (uint32_t)P7_MUL(vol, (short)PE_LoadU16(0x8009B9F8u)) >> 15);
        VX_SET16(0x118, VX_S16(0x11A));
        break;
    }
}

static int vx_transpose(int p)
{
    int pan = PE_LoadU8(0x8009D2CEu);

    if (pan != 0) {
        if (pan < 0x80)
            p = (int)((uint32_t)p + (uint32_t)(P7_MUL(p, pan) >> 7));
        else
            p = P7_MUL(p, pan) >> 8;
    }
    return p;
}

void func_80088344(pe_addr_t v, unsigned int mask)
{
    int vol, pan, p;
    unsigned int fl;
    uint16_t c;

    vol = (VX_S16(0x46) * (int)(VX_U16(0x6C) >> 8)) >> 7;
    fl = VX_U32(0x38);
    if (fl & 1u) {
        if (VX_U16(0x8A) == 0u && (c = (uint16_t)(VX_U16(0x8E) - 1u), VX_SET16(0x8E, c), c == 0u)) {
            VX_SET16(0x8E, VX_U16(0x8C));
            vx_lfo_loop(v, 0x1Cu);
            pan = P7_MUL((int)VX_U16(0x92), vx_lfo_next(v, 0x1Cu)) >> 16;
            if (pan != VX_S16(0xE8)) {
                VX_SET16(0xE8, pan);
                PE_StoreU32(v + 0xF4u, VX_U32(0xF4) | 0x10u);
                if (pan >= 0)
                    VX_SET16(0xE8, P7_SHL(pan, 1));
            }
        }
    }
    if (fl & 2u) {
        if (VX_U16(0x9E) == 0u && (c = (uint16_t)(VX_U16(0xA2) - 1u), VX_SET16(0xA2, c), c == 0u)) {
            VX_SET16(0xA2, VX_U16(0xA0));
            vx_lfo_loop(v, 0x20u);
            pan = P7_SHL(P7_MUL(vol, (int)(VX_U16(0xA6) >> 8)), 9) >> 16;
            pan = P7_MUL(pan, vx_lfo_next(v, 0x20u)) >> 15;
            if (pan != VX_S16(0xEA)) {
                VX_SET16(0xEA, pan);
                PE_StoreU32(v + 0xF4u, VX_U32(0xF4) | 3u);
            }
        }
    }
    if (fl & 4u) {
        c = (uint16_t)(VX_U16(0xB0) - 1u);
        VX_SET16(0xB0, c);
        if (c == 0u) {
            VX_SET16(0xB0, VX_U16(0xAE));
            vx_lfo_loop(v, 0x24u);
            pan = P7_MUL((int)(VX_U16(0xB4) >> 8), vx_lfo_next(v, 0x24u)) >> 15;
            if (pan != VX_S16(0xEC)) {
                VX_SET16(0xEC, pan);
                PE_StoreU32(v + 0xF4u, VX_U32(0xF4) | 3u);
            }
        }
    }
    if (fl & 0x20u) {
        vol = P7_MUL((short)(uint16_t)(VX_U16(-0x10) << 1), (int)(VX_U16(0x6C) >> 8)) >> 7;
        PE_StoreU32(v + 0xF4u, VX_U32(0xF4) | 3u);
    }
    if (VX_U32(0xF4) & 3u) {
        vol = (int)((uint32_t)vol + (uint32_t)VX_S16(0xEA));
        vol = P7_MUL(vol, (int)(PE_LoadU16(PE_LoadU32(0x8009D2C8u) + 0x4Au) & 0x7Fu)) >> 7;
        pan = ((int)(VX_U16(0x76) >> 8) + VX_S16(0xEC)) & 0xFF;
        vx_pan(v, vol, pan, mask);
        if (fl & 0x800u) {
            pe_addr_t w = 0x800B8AC0u + (uint32_t)VX_U16(0x5C) * 0x11Cu;

            PE_StoreU16(w + 0x118u, VX_U16(0x118));
            PE_StoreU16(w + 0x11Au, VX_U16(0x11A));
            vol = (int)(VX_U16(0x5E) >> 1);
            vx_pan(v, vol, pan, mask);
            PE_StoreU16(w + 0x118u, (uint16_t)(PE_LoadU16(w + 0x118u) - VX_U16(0x118)));
            PE_StoreU16(w + 0x11Au, (uint16_t)(PE_LoadU16(w + 0x11Au) - VX_U16(0x11A)));
        }
    }
    if (fl & 0x10u) {
        p = (int)VX_U16(-0x10) + VX_S16(0xE8) + VX_S16(0x36);
        p = vx_transpose(p);
        VX_SET16(0x10C, p & 0x3FFF);
        PE_StoreU32(v + 0xF4u, VX_U32(0xF4) | 0x10u);
    } else if (VX_U32(0xF4) & 0x10u) {
        p = (int)(VX_U32(0x30) + (uint32_t)VX_S16(0xE8) + (uint32_t)VX_S16(0x36));
        p = vx_transpose(p);
        VX_SET16(0x10C, p & 0x3FFF);
    }
}

/* ── src/func_80048254.c ────────────────────────────────────────────────
 * Equipment-exchange confirm.  The focused window's kind (+0x24) picks the
 * side: 6/0x1B = side 0, 0xB/0x1C = side 1, and 6/0xB also take the list
 * cursor as the entry id.  An empty entry (record + id + 0x15 == 0) goes to
 * func_800526C4.  Otherwise D_8009CF24/28 latch side/id and func_80054F58
 * decides.  1/2/5 (D_8009CFBC = r == 5, D_8009CF18 = the slot's kind != 9):
 * two or more ready party slots (D_800A1888..94 > 0) open the 0x2C chooser
 * (draw 0x80047714, update 0x8004778C, list draw 0x80050038, +0x8C
 * 0x80050020; shifted when D_8009CF0C).  Otherwise the first ready slot goes
 * to D_8009CF2C and, when the verdict fits that slot's parity, the 0x04
 * confirm window opens (0x8004790C / 0x80047A30 / 0x8004F950,
 * D_8009CF14 = 5, raised by 0x1C for odd slots); if not, func_800480AC.
 * Then func_800525EC.  0 -> func_800480AC, 4 -> message 0x63, else 7. */
extern int func_80062CC4(void);
extern pe_addr_t func_8005332C(int32_t resource_id);
extern void func_80062CB8(int a0);
extern void func_8004CC50(uint32_t first, uint32_t second);
extern void func_800480AC(void);
extern void func_800525EC(void);
extern void func_800526C4(void);

static pe_addr_t eq_entry(int side, int id)
{
    return func_8005332C(func_80059F08((uint32_t)side)) + (uint32_t)id;
}

void func_80048254(void)
{
    int side = 0, id = -1, r, i, cnt;
    pe_addr_t e, f;

    switch ((int)PE_LoadU32((pe_addr_t)func_80062CC4() + 0x24u)) {
    case 6:
        id = func_80063428((pe_addr_t)func_80062CC4());
        /* fall through */
    case 0x1B:
        side = 0;
        break;
    case 0xB:
        id = func_80063428((pe_addr_t)func_80062CC4());
        /* fall through */
    case 0x1C:
        side = 1;
        break;
    }
    if (PE_LoadU8(eq_entry(side, id) + 0x15u) == 0u) {
        func_800526C4();
        return;
    }
    PE_StoreU32(0x8009CF24u, (uint32_t)side);
    PE_StoreU32(0x8009CF28u, (uint32_t)id);
    r = func_80054F58(side, id);
    PE_StoreU32(0x8009CFBCu, (uint32_t)(r == 5));
    if (r == 1 || r == 2 || r == 5) {
        PE_StoreU32(0x8009CF18u, (uint32_t)(func_8005415C(func_80059F08((uint32_t)side)) != 9));
        cnt = ((int)PE_LoadU32(0x800A1888u) > 0) + ((int)PE_LoadU32(0x800A188Cu) > 0) +
              ((int)PE_LoadU32(0x800A1890u) > 0) + ((int)PE_LoadU32(0x800A1894u) > 0);
        if (cnt >= 2) {
            e = func_80062D2C(0x2Cu, (pe_addr_t)func_80062CC4(), 0u, 1u);
            f = func_8006322C(0x2Cu, e, e);
            PE_StoreU32(e + 0x30u, 0x80047714u);
            PE_StoreU32(e + 0x2Cu, 0x8004778Cu);
            PE_StoreU32(f + 0x30u, 0x80050038u);
            PE_StoreU32(f + 0x8Cu, 0x80050020u);
            if (PE_LoadU32(0x8009CF0Cu) != 0u) {
                PE_StoreU32(e + 0x38u, PE_LoadU32(e + 0x38u) + 0x10u);
                PE_StoreU32(f + 0x38u, 2u);
                PE_StoreU32(f + 0x58u, 2u);
                PE_StoreU32(f + 0x18u, PE_LoadU32(f + 0x18u) + 0x1Cu);
            }
            func_80062CB8((int)f);
        } else {
            for (i = 0; i < 4 && PE_LoadU32(0x800A1888u + 4u * (uint32_t)i) == 0u; i++) {
            }
            PE_StoreU32(0x8009CF2Cu, i < 4 ? (uint32_t)i : 0u);
            i = PE_LoadU8(eq_entry(side, id) + 0x15u) & 0x1F;
            if (r == 1 || (!(PE_LoadU32(0x8009CF2Cu) & 1u) ? r == 2
                                                         : (r == 5 && (unsigned int)(i - 8) >= 3u))) {
                e = func_80062D2C(4u, (pe_addr_t)func_80062CC4(), 0u, 1u);
                f = func_8006322C(4u, e, e);
                PE_StoreU32(e + 0x30u, 0x8004790Cu);
                PE_StoreU32(e + 0x2Cu, 0x80047A30u);
                PE_StoreU32(f + 0x30u, 0x8004F950u);
                PE_StoreU32(0x8009CF14u, 5u);
                func_80062CB8((int)f);
                if (PE_LoadU32(0x8009CF2Cu) & 1u) {
                    PE_StoreU32(e + 0x38u, PE_LoadU32(e + 0x38u) - 0x1Cu);
                    PE_StoreU32(f + 0x1Cu, PE_LoadU32(f + 0x1Cu) - 0x1Cu);
                }
            } else {
                func_800480AC();
            }
        }
        func_800525EC();
    } else {
        switch (r) {
        case 0:
            func_800480AC();
            break;
        case 4:
            func_8004CC50(0x63, 0);
            break;
        default:
            func_8004CC50(7, 0);
            break;
        }
    }
}
