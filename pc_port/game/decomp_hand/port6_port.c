/*
 * Hand adapters — port lane round 6 (2026-09-27).  Conventions as in
 * absent_lo_window_port.c: every guest object is a 32-bit guest address,
 * every field access is a PE_Load/PE_Store at the matched leaf's offset, and
 * a function-pointer field receives the retail code address.
 */
#include "pe_guest_decomp.h"

/* Callees with no header prototype (generated TUs / hand ports). */
void func_80063D30(pe_addr_t pe_a0);
void func_80049008(void);
void func_80048F24(void);

/* Wnd fields of src/func_80048918.c (guest layout; the f2C/f30 callbacks are
 * 32-bit code addresses). */
#define WND_KIND   0x24u
#define WND_UPDATE 0x2Cu
#define WND_DRAW   0x30u
#define WND_X      0x34u
#define WND_Y      0x38u
#define WND_F40    0x40u
#define WND_CURSOR 0x44u
#define WND_SEL    0x48u

#define G(a) PE_LoadU32(a)

/* src/func_80048918.c (0x80048918): item / equipment window constructor.
 * Builds the optional 0x36 frame (D_8009CF0C), the 0x0D/0x10 item list,
 * selects the `a2` entry, the equip-slot group (D_8009CF30: 0x18 caption,
 * 0x30 list, seven D_800C0E28 halfwords copied to D_800A18D8 with their
 * func_8005B91C table indices at D_800A18B4), the 0x0B / 0x0F detail windows,
 * then latches the item's category (it[14] & 3) and price (it+12) and runs
 * the 0x30/0x36 or default follow-up.  Previously a loud boundary in its
 * three hand callers (func_80049354, func_80047D74, func_80047A30). */
void func_80048918(int a0, int a1, int a2)
{
    pe_addr_t w, o, it, ps, pd, pb, pf;
    int i, j, k, kind;

    it = func_8005332C(a1);
    if (G(0x8009CF0Cu) != 0u) {
        w = func_80062D2C(54u, (pe_addr_t)func_80062CC4(), 0u, 0u);
        o = func_8006322C(54u, w, w);
        PE_StoreU32(w + WND_UPDATE, 0x8004EF58u);
        if (G(0x8009CF30u) != 0u)
            func_80063158(w, 0, 20);
        PE_StoreU32(o + WND_DRAW, 0x8004EF30u);
        func_80062CB8((int)o);
    }
    w = func_80062D2C(13u, (pe_addr_t)func_80062CC4(), 0u, 0u);
    o = func_8006322C(G(0x8009CF30u) != 0u ? 16u : 13u, w, w);
    PE_StoreU32(w + WND_UPDATE, 0x800494ACu);
    PE_StoreU32(w + WND_F40, 1u);
    PE_StoreU32(o + WND_DRAW, 0x8004FB48u);
    if (G(0x8009CF1Cu) != 0u) {
        func_80064B74(o, 21);
        if ((int32_t)G(o + WND_CURSOR) < 0)
            PE_StoreU32(o + WND_CURSOR, 0u);
        func_80063D30(o);
    }
    if (G(0x8009CF0Cu) != 0u) {
        func_80063158(w, 0, G(0x8009CF30u) != 0u ? -16 : 20);
        PE_StoreU32(o + WND_CURSOR, 0xFFFFFFFFu);
    }
    PE_StoreU32(0x8009CFD4u, func_80052F0C());
    func_80052E30(0u);
    PE_StoreU32(0x8009CFC4u, (uint32_t)a1);
    PE_StoreU32(0x8009CF34u, (uint32_t)(a1 == -2));
    PE_StoreU32(0x8009CF38u, (uint32_t)(a1 == -3));
    if (G(0x8009CF34u) != 0u) {
        func_80064C20(o);
        PE_StoreU32(o + WND_CURSOR, 0u);
    }
    if (G(0x8009CF1Cu) != 0u)
        func_800543CC((uint32_t)a0, -1);
    else if (G(0x8009CF34u) == 0u && G(0x8009CF38u) == 0u)
        func_800542A0((uint32_t)a0);
    func_800647D0(o, func_80054288());
    if (a2 >= 0) {
        i = 0;
        while (i < func_80054288()) {
            if (func_800556E8(i) == a2)
                break;
            i++;
        }
        if (i < func_80054288()) {
            PE_StoreU32(o + WND_SEL, (uint32_t)i);
            func_80063D30(o);
        }
    }
    if ((int32_t)G(o + WND_CURSOR) >= 0 && G(0x8009CF0Cu) == 0u)
        func_80062CB8((int)o);
    if (G(0x8009CF30u) != 0u) {
        func_80063158(w, 0, 56);
        PE_StoreU32(w + WND_Y, G(w + WND_Y) - 48u);
        w = func_80062D2C(24u, 0u, 0u, 0u);
        PE_StoreU32(w + WND_DRAW, 0x8004905Cu);
        PE_StoreU32(w + WND_X, G(w + WND_X) + 20u);
        func_80063158(w, -136, 0);
        if (G(0x8009CF0Cu) == 0u) {
            uint32_t neg = G(o + WND_CURSOR) >> 31;

            w = func_80062D2C(48u, func_80062A34(2u, 0u), 0u, 0u);
            o = func_8006322C(48u, w, w);
            PE_StoreU32(w + WND_UPDATE, 0x800491C8u);
            PE_StoreU32(o + WND_DRAW, 0x80050060u);
            if (neg) {
                PE_StoreU32(o + WND_SEL, 0u);
                PE_StoreU32(o + WND_CURSOR, 0u);
            }
            if ((int32_t)G(o + WND_CURSOR) >= 0)
                func_80062CB8((int)o);
        }
        ps = 0x800C0E28u;
        pd = 0x800A18D8u;
        pb = 0x800A18B4u;
        pf = 0x800A18FCu;
        for (j = 0; j < 7; j++) {
            PE_StoreU32(pd, PE_LoadU16(ps));
            ps += 2u;
            PE_StoreU32(pf, 0u);
            func_8005B91C(j, (int32_t)G(pd), pb, 0u);
            pd += 4u;
            pb += 4u;
            pf += 4u;
        }
        PE_StoreU32(0x8009CF80u, 0u);
        PE_StoreU32(0x8009CF40u, 0u);
        PE_StoreU32(0x8009CF68u, G(0x800C0E10u));
        for (k = 6; k >= 0; k--)
            PE_StoreU32(0x800A1898u + (uint32_t)k * 4u, 0u);
        func_8004BF08();
    }
    func_800631AC(func_80062A34(1u, 1u));
    func_800631AC(func_80062A34(1u, 27u));
    if (G(0x8009CF1Cu) != 0u) {
        if (func_80062A34(1u, 47u) == 0u) {
            w = func_80062D2C(47u, 0u, 0u, 0u);
            PE_StoreU32(w + WND_DRAW, 0x80045FA4u);
        }
    }
    w = func_80062D2C(11u, o, 0u, 0u);
    o = func_8006322C(11u, w, w);
    PE_StoreU32(w + WND_F40, 1u);
    PE_StoreU32(o + WND_DRAW, 0x8004F9A0u);
    func_80064C20(o);
    func_80063158(w, 6, 0);
    w = func_80062D2C(15u, o, 0u, 0u);
    if (G(0x8009CF30u) != 0u) {
        o = func_8006322C(29u, w, w);
        PE_StoreU32(w + WND_UPDATE, 0x80049354u);
        PE_StoreU32(o + WND_DRAW, 0x80050088u);
        PE_StoreU32(o + WND_CURSOR, 0xFFFFFFFFu);
        PE_StoreU8(0x800A1960u, 0u);
    }
    PE_StoreU32(w + WND_DRAW, 0x8004A0C8u);
    PE_StoreU32(0x8009CFC8u, it != 0u ? (PE_LoadU8(it + 14u) & 3u) : 0u);
    PE_StoreU32(0x8009CFCCu, it != 0u ? PE_LoadU16(it + 12u) : 0u);
    PE_StoreU32(0x8009CF18u, (uint32_t)(a0 != 0x200));
    kind = (int)G((pe_addr_t)func_80062CC4() + WND_KIND);
    if (kind == 48 || kind == 54)
        func_80049008();
    else
        func_80048F24();
}

/* src/func_8007E4E0.c (0x8007E4E0): copy the words of
 * [func_8007E334 + 0x10, func_8007E3B4) — 28 words of retail code in guest
 * RAM — to 0x0000DF80, the KUSEG alias of low RAM 0x8000DF80 (the BIOS
 * exception-return stub the SIO/pad handler jumps through at 0xDFFC). */
void func_8007E4E0(void)
{
    pe_addr_t dst = 0x0000DF80u;
    pe_addr_t src = 0x8007E334u + 0x10u;

    do {
        PE_StoreU32(dst, PE_LoadU32(src));
        src += 4u;
        dst += 4u;
    } while (src != 0x8007E3B4u);
}

/* src/func_8004EF58.c (0x8004EF58): input handler of the 0x36 item frame
 * window (installed by func_80048918 at +0x2C).  Confirm/select (0x14000)
 * with a non-empty item list focuses list 7 — or, when it does not exist,
 * re-lays out the 0x0D/0x0F/0x0B/0x2F windows to the column x (176 in the
 * equip-slot mode, else 162/156 by the list's +0x80 flag) and focuses the
 * 0x0D list — and, when list 7 existed, opens the 0x35 window (draw
 * 0x8004FA10).  Cancel (0x40) closes back to the 0x05-group caller
 * (func_80048918(830, -1, -1)) or tears the item windows down and returns
 * to the menu page (func_8004E704).  W fields: +0x04 parent, +0x18 x,
 * +0x30 draw, +0x44/+0x48 cursor/selection, +0x80 flag. */
int func_8004EF58(pe_addr_t self, int key)
{
    pe_addr_t l, w, v;
    int fresh, dx;

    (void)func_80062A20(self, 0u);
    if (key & 0x14000) {
        if (func_80054288() != 0) {
            l = func_80062A34(2u, 7u);
            fresh = l != 0u;
            if (l == 0u) {
                l = func_80062A20(func_80062A34(1u, 13u), 0u);
                w = func_80062A34(1u, 15u);
                v = func_80062A20(func_80062A34(1u, 13u), 0u);
                dx = (G(0x8009CF30u) != 0u ? 176 : (G(v + 0x80u) != 0u ? 162 : 156))
                     - (int)G(w + 0x18u);
                func_80063158(w, dx, 0);
                func_80063198(w);
                w = func_80062A34(1u, 11u);
                func_80063158(w, dx, 0);
                func_80063198(w);
                w = func_80062A34(1u, 47u);
                func_80063158(w, dx, 0);
                func_80063198(w);
            }
            PE_StoreU32(l + WND_CURSOR, 0u);
            PE_StoreU32(l + WND_SEL, 0u);
            func_80063D30(l);
            func_80062CB8((int)l);
            if (fresh) {
                self = func_80062D2C(53u, 0u, 0u, 0u);
                PE_StoreU32(func_8006322C(53u, self, self) + WND_DRAW, 0x8004FA10u);
            }
            func_800525EC();
        } else {
            func_800526C4();
        }
    }
    if (key & 0x40) {
        func_80062F1C(self);
        if (func_80062A34(1u, 5u) != 0u) {
            func_80062F3C(5u);
            func_80062F3C(6u);
            func_80062F3C(7u);
            func_80062F3C(53u);
            func_80048918(830, -1, -1);
        } else {
            func_80062F3C(15u);
            func_80062F3C(11u);
            func_80062F3C(13u);
            func_80062F3C(24u);
            func_80062F3C(48u);
            if (G(0x8009CF0Cu) != 0u) {
                func_80062CB8((int)func_80062A34(2u, 50u));
            } else {
                if (func_80062CC4() != 0)
                    func_80063198(G((pe_addr_t)func_80062CC4() + 4u));
                func_80063198(func_80062A34(1u, 27u));
            }
            PE_StoreU32(0x8009CF34u, 0u);
            func_80062F3C(47u);
            PE_StoreU32(0x8009CF1Cu, 0u);
            PE_StoreU32(0x8009CF30u, 0u);
            func_8004E704((int)G(0x8009CF0Cu) - 1);
        }
        func_80052634();
    }
    return 1;
}
