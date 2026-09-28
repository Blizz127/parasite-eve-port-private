/*
 * Hand adapters — port_absent lane round 3 (2026-09-24): leaves matched in
 * the batch-7 gate that the generator cannot derive (rejection reasons in
 * build/fanout/portabsent/gencheck8.txt).  Authority for every line: the
 * matched src/func_X.c named in each comment.  Conventions as in the other
 * decomp_hand/absent_*_port.c files: guest pointers are pe_addr_t, guest
 * pointers held in RAM are loaded with PE_LoadU32, guest-layout structs are
 * read at the leaf's own offsets and widths, retail stack buffers handed to
 * guest-address callees live at PE_HAND_R3_STACK, and a callee with no
 * pc_port implementation is a loud boundary.  Tests:
 * pc_port/tests/test_pa_r3.h.
 */
#include "pe_guest_decomp.h"
#include "hand_absent_r3_protos.h"

/* decomp-derived callees without a canonical header prototype (generated
 * TUs in pc_port/game/decomp/; signatures copied from those definitions). */
int func_80038D0C(void);
unsigned int func_8006E3D4(pe_addr_t pe_a0);
void func_800375D0(int a0);
void func_8005E894(int a0, int a1);
void func_800E00CC(pe_addr_t pe_a0, unsigned char a1, short a2, unsigned char a3,
                   unsigned char a4, unsigned char a5, unsigned char a6);

/* Retail reads through a zero record pointer land in low main RAM via the
 * KUSEG mirror; PE_Translate folds KUSEG onto KSEG0 (PE_RamCanonical), so
 * this is the identity — kept as a marker of those reads. */
static pe_addr_t r3_ram(pe_addr_t a)
{
    return a;
}

/* field-VM operand k of an `int **a` / `unsigned char **a0` vector:
 * the element is a 32-bit guest pointer at a + 4k. */
static pe_addr_t r3_arg(pe_addr_t a, unsigned k)
{
    return PE_LoadU32(a + 4u * k);
}

/* ── src/func_80015964.c ────────────────────────────────────────────────
 * Field-VM opcode: if !func_80038D0C() run func_80038D74 (no pc_port
 * implementation: boundary).  k = func_8003944C(**a0).  k == 0xFF: reload
 * func_80039970 and D_8009D280 = func_8006E3D4(D_8009CD78 when
 * func_800392EC() < 2, else D_8009CD80); return 1.  Otherwise row =
 * (u8)(func_800392EC() - 1) / 10; D_8009D280 = func_8006E3D4(
 * &D_8009180C[D_8009173C[row][k]]) (rows of 24 bytes, 8-byte entries); when
 * that equals D_8009D1C4 set D_8009D1A0 |= 0x2000 and D_800B0CD8 |= 0x800.
 * Returns 1.  (The generator emitted `unsigned char *p = D_8009173C[row]`
 * as a scalar load and failed to compile.) */
int func_80015964(pe_addr_t a0)
{
    unsigned char k;
    unsigned char row;

    if (!(unsigned char)func_80038D0C())
        (void)PE_D_COMP_BOUNDARY0("func_80038D74", 0x80038D74u);
    k = func_8003944C(PE_LoadU8(PE_LoadU32(a0)));
    if (k == 0xFFu) {
        if (func_800392EC() < 2u) {
            (void)func_80039970();
            PE_StoreU32(0x8009D280u, func_8006E3D4(0x8009CD78u));
        } else {
            (void)func_80039970();
            PE_StoreU32(0x8009D280u, func_8006E3D4(0x8009CD80u));
        }
        return 1;
    }
    row = (unsigned char)(func_800392EC() - 1u);
    row /= 10;
    {
        pe_addr_t p = 0x8009173Cu + (uint32_t)row * 24u;
        PE_StoreU32(0x8009D280u,
                    func_8006E3D4(0x8009180Cu + (uint32_t)PE_LoadU8(p + k) * 8u));
    }
    if ((int)PE_LoadU32(0x8009D1C4u) != (int)PE_LoadU32(0x8009D280u))
        return 1;
    PE_StoreU32(0x8009D1A0u, PE_LoadU32(0x8009D1A0u) | 0x2000u);
    PE_StoreU32(0x800B0CD8u, PE_LoadU32(0x800B0CD8u) | 0x800u);
    return 1;
}

/* ── src/func_80016910.c ────────────────────────────────────────────────
 * Field-VM system opcode dispatched on *a[0] (a = `int **` operand vector,
 * *a[k] = PE_LoadU32(PE_LoadU32(a + 4k))).  Ent/Vm are guest layout (E18):
 * next at +4, fC/fD bytes, f28/f2C/f30 ints, f98 flags, f27C/f27D bytes;
 * Vm f10 at +0x10.  The short stack triples b1/b2 and the 2-byte d are
 * retail stack buffers: PE_HAND_R3_STACK + 0x00 / + 0x08 / + 0x10.
 * Returns 1, except 0xC1C when func_8006914C(1) == 1 (returns 0). */
#define R3_B1 (PE_HAND_R3_STACK + 0x00u)
#define R3_B2 (PE_HAND_R3_STACK + 0x08u)
#define R3_D  (PE_HAND_R3_STACK + 0x10u)

static int r3_op(pe_addr_t a, unsigned k)
{
    return (int)PE_LoadU32(r3_arg(a, k));
}

int func_80016910(pe_addr_t a)
{
    pe_addr_t e;
    int r;

    switch (r3_op(a, 0)) {
    case 0x7D0:
        break;
    case 0x835:
        PE_StoreU32(0x800B0CD8u, PE_LoadU32(0x800B0CD8u) | 0x800u);
        break;
    case 0x898:
        (void)func_800665A0(PE_LoadU32(0x8009D254u) + 0x28u, r3_op(a, 1), r3_op(a, 2));
        break;
    case 0x899:
        PE_StoreU16(0x800BD028u, (uint16_t)r3_op(a, 1));
        PE_StoreU16(0x800BD02Au, (uint16_t)r3_op(a, 2));
        break;
    case 0x8FC:
        func_800375D0(r3_op(a, 1) != 0);
        break;
    case 0x960:
        PE_StoreU16(R3_B1 + 0u, (uint16_t)(short)(r3_op(a, 1) >> 16));
        PE_StoreU16(R3_B1 + 2u, (uint16_t)(short)(r3_op(a, 2) >> 16));
        PE_StoreU16(R3_B1 + 4u, (uint16_t)(short)(r3_op(a, 3) >> 16));
        func_800E00CC(R3_B1, 0, (short)PE_LoadU16(r3_arg(a, 4)),
                      PE_LoadU8(r3_arg(a, 5)), 0, 0, 0);
        break;
    case 0x961:
        PE_StoreU8(0x8009CDF8u, (uint8_t)r3_op(a, 1));
        PE_StoreU8(0x8009CDF9u, (uint8_t)r3_op(a, 2));
        PE_StoreU8(0x8009CDFAu, (uint8_t)r3_op(a, 3));
        break;
    case 0x962:
        PE_StoreU16(R3_B2 + 0u, (uint16_t)(short)(r3_op(a, 1) >> 16));
        PE_StoreU16(R3_B2 + 2u, (uint16_t)(short)(r3_op(a, 2) >> 16));
        PE_StoreU16(R3_B2 + 4u, (uint16_t)(short)(r3_op(a, 3) >> 16));
        func_800E00CC(R3_B2, 1, (short)PE_LoadU16(r3_arg(a, 4)),
                      PE_LoadU8(r3_arg(a, 5)), PE_LoadU8(0x8009CDF8u),
                      PE_LoadU8(0x8009CDF9u), PE_LoadU8(0x8009CDFAu));
        break;
    case 0x9C4:
        PE_StoreU8(R3_D + 0u, (uint8_t)r3_op(a, 1));
        PE_StoreU8(R3_D + 1u, (uint8_t)r3_op(a, 2));
        {
            pe_addr_t n = func_80035038(R3_D, PE_LoadU32(0x8009D2F0u), 0u);
            PE_StoreU32(n + 0x28u, (uint32_t)r3_op(a, 3));
            PE_StoreU32(n + 0x2Cu, (uint32_t)r3_op(a, 4));
            PE_StoreU32(n + 0x30u, (uint32_t)r3_op(a, 5));
        }
        break;
    case 0xA28:
        PE_StoreU8(PE_LoadU32(0x8009D2F0u) + 0x27Cu, (uint8_t)r3_op(a, 1));
        break;
    case 0xA29:
        PE_StoreU8(PE_LoadU32(0x8009D2F0u) + 0x27Du, (uint8_t)r3_op(a, 1));
        break;
    case 0xA8C:
        PE_StoreU8(0x800BD025u, (uint8_t)r3_op(a, 1));
        PE_StoreU8(0x800BD026u, (uint8_t)r3_op(a, 2));
        PE_StoreU8(0x800BD027u, (uint8_t)r3_op(a, 3));
        break;
    case 0xAF0:
        func_80033A40();
        break;
    case 0xAF1:
        func_800339A0(PE_LoadU8(r3_arg(a, 1)));
        break;
    case 0xB54:
        PE_StoreU32(0x800B0CD8u, PE_LoadU32(0x800B0CD8u) | 0x400000u);
        break;
    case 0xBB8:
        if (r3_op(a, 1) != 0) {
            for (e = PE_LoadU32(0x8009D20Cu); e != 0; e = PE_LoadU32(e + 4u)) {
                if ((int)PE_LoadU8(e + 0xCu) == r3_op(a, 1) &&
                    (int)PE_LoadU8(e + 0xDu) == r3_op(a, 2) &&
                    !(PE_LoadU32(e + 0x98u) & 0x10u))
                    return 1;
            }
        }
        break;
    case 0xC1C:
        r = func_8006914C(1);
        if (r == 1) {
            PE_StoreU32(0x8009CE00u, PE_LoadU32(0x8009CE00u) - 0x28u);
            PE_StoreU32(PE_LoadU32(0x8009D300u) + 0x10u, (uint32_t)r);
            return 0;
        }
        break;
    case 0xC80:
        PE_StoreU32(0x800B0CD8u, PE_LoadU32(0x800B0CD8u) | 0x8000000u);
        break;
    }
    return 1;
}

/* ── src/func_8003495C.c ────────────────────────────────────────────────
 * Area-title banner state machine in D_8009D1AC (bits 8-9 mode, 10-11
 * variant, low byte a frame countdown, bits 12-18 pending titles).  With no
 * live object (D_8009D1A8 == 0, *D_8009D1A8 == 0 or its +0x10 <= 0):
 * func_800374E8() and clear the mode.  Mode 1: buf = {obj+0x10, obj+0x88,
 * 0} (shorts; retail stack, here PE_HAND_R3_STACK + 0x18), func_800374E8,
 * func_80037454(func_8005BCB0() ? 0x14 : 0x61, D_8009CE80 < 2 ? 0xF :
 * 0xC3, 0, 0), func_800375E0(0, 2, buf); title D_80091464 (variant 0) or
 * D_80091474 (variant == 1); D_800BCEA8 = 2; mode += 1 (mod 4);
 * D_800BCEB4 |= 0x2000000.  Mode 2 with the countdown at 0: mode = 2,
 * countdown 0x4B, and the lowest pending title bit selects row
 * func_8005BCB0() of its string table (D_80091480/22, D_800914AC/20,
 * D_800914D4/20, D_800914FC/18, D_80091520/18, D_80091544/22, D_80091570/21
 * byte rows) and clears it; none pending: func_800374E8 and clear the mode.
 * Always: countdown -= 1 (low byte, wraps), func_8005E894(0, D_8009CE80 < 2
 * ? 0xB : 0xBF), func_80061C34(0x140, 0x14, 0, 0). */
#define R3_BUF (PE_HAND_R3_STACK + 0x18u)
#define R3_FLAGS 0x8009D1ACu

void func_8003495C(void)
{
    pe_addr_t slot = PE_LoadU32(0x8009D1A8u);
    unsigned int mode;
    unsigned int v;
    unsigned int f;

    if (slot == 0 || PE_LoadU32(slot) == 0 ||
        (int)PE_LoadU32(PE_LoadU32(slot) + 0x10u) <= 0) {
        func_800374E8();
        PE_StoreU32(R3_FLAGS, PE_LoadU32(R3_FLAGS) & ~0x300u);
    }
    mode = (PE_LoadU32(R3_FLAGS) >> 8) & 3u;
    switch (mode) {
    case 1: {
        pe_addr_t obj = PE_LoadU32(PE_LoadU32(0x8009D1A8u));
        int x, y;

        PE_StoreU16(R3_BUF + 0u, (uint16_t)PE_LoadU32(obj + 0x10u));
        obj = PE_LoadU32(PE_LoadU32(0x8009D1A8u));
        PE_StoreU16(R3_BUF + 2u, (uint16_t)PE_LoadU32(obj + 0x88u));
        PE_StoreU16(R3_BUF + 4u, 0u);
        func_800374E8();
        x = func_8005BCB0() ? 0x14 : 0x61;
        y = PE_LoadU8(0x8009CE80u) < 2u ? 0xF : 0xC3;
        func_80037454(x, y, 0, 0);
        func_800375E0(0, 2u, R3_BUF);
        v = (PE_LoadU32(R3_FLAGS) >> 10) & 3u;
        if (v != 0) {
            if (v == mode)
                PE_StoreU32(0x800BCEACu, 0x80091474u);
        } else {
            PE_StoreU32(0x800BCEACu, 0x80091464u);
        }
        PE_StoreU8(0x800BCEA8u, 2u);
        {
            uint32_t w = PE_LoadU32(R3_FLAGS);
            uint32_t t = w & ~0x300u;
            PE_StoreU32(R3_FLAGS, t | ((((w >> 8) & 3u) + 1u) & 3u) << 8);
        }
        PE_StoreU32(0x800BCEB4u, PE_LoadU32(0x800BCEB4u) | 0x2000000u);
        break;
    }
    case 2:
        if (PE_LoadU8(R3_FLAGS) == 0u) {
            static const struct { uint32_t bit; pe_addr_t table; uint32_t stride; } titles[] = {
                { 0x1000u,  0x80091480u, 22u }, { 0x2000u,  0x800914ACu, 20u },
                { 0x4000u,  0x800914D4u, 20u }, { 0x8000u,  0x800914FCu, 18u },
                { 0x10000u, 0x80091520u, 18u }, { 0x20000u, 0x80091544u, 22u },
                { 0x40000u, 0x80091570u, 21u },
            };
            unsigned t;

            PE_StoreU32(R3_FLAGS, (PE_LoadU32(R3_FLAGS) & ~0x300u) | 0x200u);
            PE_StoreU8(R3_FLAGS, 0x4Bu);
            f = PE_LoadU32(R3_FLAGS);
            for (t = 0; t < sizeof titles / sizeof titles[0]; t++) {
                if (f & titles[t].bit) {
                    PE_StoreU32(0x800BCEACu, titles[t].table +
                                (uint32_t)func_8005BCB0() * titles[t].stride);
                    PE_StoreU32(R3_FLAGS, PE_LoadU32(R3_FLAGS) & ~titles[t].bit);
                    break;
                }
            }
            if (t == sizeof titles / sizeof titles[0]) {
                func_800374E8();
                PE_StoreU32(R3_FLAGS, PE_LoadU32(R3_FLAGS) & ~0x300u);
            }
        }
        break;
    }
    PE_StoreU8(R3_FLAGS, (uint8_t)(PE_LoadU8(R3_FLAGS) - 1u));
    func_8005E894(0, PE_LoadU8(0x8009CE80u) < 2u ? 0xB : 0xBF);
    func_80061C34(0x140u, 0x14u, 0u, 0u);
}

/* ── src/func_80042020.c ────────────────────────────────────────────────
 * Memory-card slot commit.  CardRec (D_800A0ED4[slot], 0x418 bytes): sel
 * +0, f1..fB bytes, count +0xA, h14/h16 +0x14/+0x16, 15 CardEntry at
 * +0x1C of 0x44 bytes (state +0, f1 +1, f29 +0x29).  Only when sel == 1:
 * func_80042798(); func_8005C25C(); count += (entry n's state != 1);
 * f3 = n; sprintf the file name into D_8009EE70 with format D_80092224 and
 * (D_800A0ED4 < c, entry[f3].f29 + '0', f3 + 'A'); func_80040B80(
 * &D_8009EE70); f5 = n, f6 = 0, entry.f1 = 0, f1 = 1, f7 = 1, h14 =
 * 0x2000, h16 = 10, D_800A1854 = c, D_800A1858 = 0x2000, fB = entry.state
 * == 1 ? 0xB : 4, entry.state = 1.
 * func_80071A84 is the variadic sprintf (pe_dis.sh 0x80071A84 0x10: it
 * spills a1..a3 to the caller frame and reserves 0x250 bytes) with no
 * pc_port implementation: loud boundary.  The boundary vector holds four
 * registers; the fifth argument (f3 + 'A', a stack word in retail) is
 * computed but cannot be recorded. */
#define R3_CARD_BASE 0x800A0ED4u
#define R3_CARD_SIZE 0x418u

void func_80042020(int slot, int n)
{
    pe_addr_t c = R3_CARD_BASE + (uint32_t)slot * R3_CARD_SIZE;
    pe_addr_t e;
    unsigned int st;

    st = PE_LoadU8(c + 0u);
    if (st != 1u)
        return;
    e = c + 0x1Cu + (uint32_t)n * 0x44u;
    func_80042798();
    func_8005C25C();
    {
        int m = PE_LoadU8(c + 0xAu);
        PE_StoreU8(c + 0xAu, (uint8_t)((PE_LoadU8(e + 0u) != st) ? m + 1 : m));
    }
    PE_StoreU8(c + 3u, (uint8_t)n);
    {
        unsigned f3 = PE_LoadU8(c + 3u);
        int fifth = (int)f3 + 'A';

        (void)PE_D_COMP_BOUNDARY4("func_80071A84", 0x80071A84u, 0x8009EE70u,
                                  PE_LoadU32(0x80092224u),
                                  R3_CARD_BASE < c ? 1 : 0,
                                  PE_LoadU8(c + 0x1Cu + f3 * 0x44u + 0x29u) + '0');
        (void)fifth;
    }
    (void)func_80040B80(0x8009EE70u);
    PE_StoreU8(c + 5u, (uint8_t)n);
    PE_StoreU8(c + 6u, 0u);
    PE_StoreU8(e + 1u, 0u);
    PE_StoreU8(c + 1u, 1u);
    PE_StoreU8(c + 7u, 1u);
    PE_StoreU16(c + 0x14u, 0x2000u);
    PE_StoreU16(c + 0x16u, 10u);
    PE_StoreU32(0x800A1854u, c);
    PE_StoreU32(0x800A1858u, 0x2000u);
    PE_StoreU8(c + 0xBu, PE_LoadU8(e + 0u) == 1u ? 0xBu : 4u);
    PE_StoreU8(e + 0u, 1u);
}

/* ── src/func_8004A0C8.c ────────────────────────────────────────────────
 * Equipment detail panel.  Selection: window (2,0xD) if present (its
 * +0x44 cursor >= 0 selects func_800556E8(func_80063428(w))); else in the
 * weapon list (D_8009CF30) window (2,0x10) likewise; else outside the
 * lists (D_8009CF1C == 0) the equipped weapon/armour D_800C0E20 /
 * D_800C0E22 (signed char) by D_8009CF18.  id = func_80059F08(0); icon
 * func_800536B8(id) at +4,+4; it = func_8005332C(id); D_8009CF18 =
 * it->f6 != 9; func_8004551C(it).  Inside a list/compare mode draw the
 * three base (f9/f8/f7 bytes) + bonus (f12/f10/fE shorts) pairs and the
 * 0x87/0x88 glyphs.  D_8009CFC4 >= 0: the compare column — current fE/f10/
 * f12, then each plus D_8009CFCC when D_8009CFC8 selects it, clamped to
 * 999, coloured by func_8005E988(old, new); colour reset 0x808080.
 * Finally window (1,0xB) is shown (func_80063198) when it->f14 else hidden
 * (func_800631AC).  Item is guest layout: f6..f9 bytes, fE/f10/f12 shorts,
 * f14 byte; a 0 record is read through the KUSEG mirror as retail does. */
static int r3_stat(pe_addr_t it, uint32_t off, int k)
{
    int base = (short)PE_LoadU16(r3_ram(it + off));
    int add = ((int)PE_LoadU32(0x8009CFC8u) == k) ? (int)PE_LoadU32(0x8009CFCCu) : 0;
    return (base + add < 1000) ? base + add : 999;
}

void func_8004A0C8(void)
{
    pe_addr_t w;
    pe_addr_t it;
    int id;
    int v;

    w = func_80062A34(2u, 0xDu);
    if (w != 0) {
        if ((int)PE_LoadU32(w + 0x44u) >= 0)
            func_80059EC8(0u, func_800556E8(func_80063428(w)));
    } else if (PE_LoadU32(0x8009CF30u) != 0) {
        w = func_80062A34(2u, 0x10u);
        if (w != 0) {
            if ((int)PE_LoadU32(w + 0x44u) >= 0)
                func_80059EC8(0u, func_800556E8(func_80063428(w)));
        }
    } else if (PE_LoadU32(0x8009CF1Cu) == 0) {
        if (PE_LoadU32(0x8009CF18u) != 0)
            func_80059EC8(0u, (int8_t)PE_LoadU8(0x800C0E20u));
        else
            func_80059EC8(0u, (int8_t)PE_LoadU8(0x800C0E22u));
    }
    id = func_80059F08(0u);
    func_8005E8A4(4, 4);
    func_800536B8(id);
    func_8005E8A4(-4, -4);
    it = func_8005332C(id);
    PE_StoreU32(0x8009CF18u, PE_LoadU8(r3_ram(it + 6u)) != 9u);
    func_8004551C(it);
    if (PE_LoadU32(0x8009CF1Cu) != 0 || PE_LoadU32(0x8009CF30u) != 0 ||
        PE_LoadU32(0x8009CF34u) != 0 || PE_LoadU32(0x8009CF38u) != 0) {
        if (it != 0) {
            func_8005E8A4(0x2A, -0xC);
            func_8005FDF0(PE_LoadU8(it + 9u));
            func_8005E8A4(5, 0);
            func_8005FF28((short)PE_LoadU16(it + 0x12u));
            func_8005E8A4(-0x2D, -0xE);
            func_8005FDF0(PE_LoadU8(it + 8u));
            func_8005E8A4(5, 0);
            func_8005FF28((short)PE_LoadU16(it + 0x10u));
            func_8005E8A4(-0x2D, -0xE);
            func_8005FDF0(PE_LoadU8(it + 7u));
            func_8005E8A4(5, 0);
            func_8005FF28((short)PE_LoadU16(it + 0xEu));
            func_8005E8A4(-0x2D, -0xA);
            func_8005EB64(0x87u);
            func_8005E8A4(0x19, 0);
            func_8005EB64(0x88u);
        }
    }
    if ((int)PE_LoadU32(0x8009CFC4u) >= 0) {
        func_8005E8A4(0x28, -0x32);
        func_8005EB64(0x88u);
        func_8005E8A4(0, 10);
        func_8005FF28((short)PE_LoadU16(r3_ram(it + 0xEu)));
        func_8005E8A4(-0x14, 0xE);
        func_8005FF28((short)PE_LoadU16(r3_ram(it + 0x10u)));
        func_8005E8A4(-0x14, 0xE);
        func_8005FF28((short)PE_LoadU16(r3_ram(it + 0x12u)));
        func_8005EB64(0x22u);
        func_8005E8A4(0, -0xE);
        func_8005EB64(0x22u);
        func_8005E8A4(0, -0xE);
        func_8005EB64(0x22u);
        func_8005E8A4(10, -10);
        func_8005EB64(0x88u);
        func_8005E8A4(0, 10);
        v = r3_stat(it, 0xEu, 0);
        func_8005E988((short)PE_LoadU16(r3_ram(it + 0xEu)), v);
        func_8005FF28(v);
        func_8005E8A4(-0x14, 0xE);
        v = r3_stat(it, 0x10u, 1);
        func_8005E988((short)PE_LoadU16(r3_ram(it + 0x10u)), v);
        func_8005FF28(v);
        func_8005E8A4(-0x14, 0xE);
        v = r3_stat(it, 0x12u, 2);
        func_8005E988((short)PE_LoadU16(r3_ram(it + 0x12u)), v);
        func_8005FF28(v);
        func_8005E968(0x808080);
    }
    if (PE_LoadU8(r3_ram(it + 0x14u)) != 0u)
        func_80063198(func_80062A34(1u, 0xBu));
    else
        func_800631AC(func_80062A34(1u, 0xBu));
}

/* ── src/func_8005C688.c ────────────────────────────────────────────────
 * Build the "equippable" selection bitmap: list D_8009D048 = D_800C0E48,
 * count D_8009D050 = func_80052F70(), bitmap D_8009D058 = D_8009D05C (2
 * words, cleared after func_800542A0(D_8009D0CC ? 0x1FE : 0x200)).  For
 * every slot k < D_8009D040: the record of id D_800A1D9C[k] (signed short;
 * the inline rec(): list entry v = D_8009D048[id] for 0 <= id <
 * D_8009D050, 0x100..0x17F -> D_800BEEAC + v*32, 1..0xFF ->
 * func_8005DB44(v-1), 0x200..0x208 -> D_8009DE64 + v*32, else 0) qualifies
 * when r[7]+*(short *)(r+0xE) >= D_8009D0D0, r[8]+(r+0x10) >= min_b,
 * r[9]+(r+0x12) >= min_c and r[4] is neither 0x93 nor 0x61; each sets its
 * bit and counts.  With a count, it is negated when some D_800C0EAC slot
 * (128 x 32 bytes: f0, f5 bit 0x10, f6) is in use with f6 < 9 (D_8009D0CC)
 * or f6 == 9.  D_8009D068 = n != 0; returns n.  A 0 record is read through
 * the KUSEG mirror as retail does. */
static pe_addr_t r3_rec(int a0)
{
    int v;
    int w;

    if (a0 >= 0 && a0 < (int)PE_LoadU32(0x8009D050u)) {
        v = (short)PE_LoadU16(PE_LoadU32(0x8009D048u) + (uint32_t)a0 * 2u);
        w = v;
        if ((unsigned int)(v - 0x100) < 0x80u)
            return (pe_addr_t)((uint32_t)(v << 5) + 0x800BEEACu);
        if ((unsigned int)(v - 1) < 0xFFu)
            return func_8005DB44((unsigned int)(v - 1));
        if ((unsigned int)(w - 0x200) < 9u)
            return (pe_addr_t)((uint32_t)(w << 5) + 0x8009DE64u);
        return 0;
    }
    return 0;
}

int func_8005C688(int min_b, int min_c)
{
    int i;
    int n;
    int k;
    int id;
    pe_addr_t r;
    pe_addr_t p;

    PE_StoreU32(0x8009D048u, 0x800C0E48u);
    PE_StoreU32(0x8009D050u, func_80052F70());
    PE_StoreU32(0x8009D058u, 0x8009D05Cu);
    PE_StoreU32(0x8009D064u, 2u);
    func_800542A0(PE_LoadU32(0x8009D0CCu) != 0 ? 0x1FEu : 0x200u);
    for (i = 0; i < (int)PE_LoadU32(0x8009D064u); i++)
        PE_StoreU32(PE_LoadU32(0x8009D058u) + (uint32_t)i * 4u, 0u);
    n = 0;
    for (k = 0; k < (int)PE_LoadU32(0x8009D040u); k++) {
        id = (k >= 0 && k < (int)PE_LoadU32(0x8009D040u))
             ? (short)PE_LoadU16(0x800A1D9Cu + (uint32_t)k * 2u) : 0;
        r = r3_rec(id);
        if ((int)PE_LoadU8(r3_ram(r + 7u)) + (short)PE_LoadU16(r3_ram(r + 0xEu)) <
            (int)PE_LoadU32(0x8009D0D0u))
            continue;
        if ((int)PE_LoadU8(r3_ram(r + 8u)) + (short)PE_LoadU16(r3_ram(r + 0x10u)) < min_b)
            continue;
        if ((int)PE_LoadU8(r3_ram(r + 9u)) + (short)PE_LoadU16(r3_ram(r + 0x12u)) < min_c)
            continue;
        if (PE_LoadU8(r3_ram(r + 4u)) == 0x93u || PE_LoadU8(r3_ram(r + 4u)) == 0x61u)
            continue;
        n++;
        {
            pe_addr_t word = PE_LoadU32(0x8009D058u) + (uint32_t)(k >> 5) * 4u;
            PE_StoreU32(word, PE_LoadU32(word) | (1u << (k & 0x1F)));
        }
    }
    if (n != 0) {
        const pe_addr_t end = 0x800C0EACu + 128u * 32u;
        int armour = PE_LoadU32(0x8009D0CCu) != 0;

        for (p = 0x800C0EACu; p < end; p += 32u) {
            unsigned f6 = PE_LoadU8(p + 6u);
            if (PE_LoadU8(p + 0u) && (armour ? f6 < 9u : f6 == 9u) &&
                (PE_LoadU8(p + 5u) & 0x10u))
                break;
        }
        if (p < end)
            n = -n;
    }
    PE_StoreU32(0x8009D068u, n != 0);
    return n;
}

/* ── src/func_8007A360.c: a0 == 2 -> func_8007BBB0(), 1; else 0 when
 * func_8007BBFC() != 0, or when a0 == 1 and func_8007BAC0() != 0; else 1. */
int func_8007A360(int a0)
{
    if (a0 == 2) {
        func_8007BBB0();
        return 1;
    }
    if (func_8007BBFC() != 0)
        return 0;
    if (a0 == 1 && func_8007BAC0() != 0)
        return 0;
    return 1;
}

/* ── src/func_80081D74.c: card-transfer job start.  Busy (D_8009B70C == 1)
 * -> 0.  Else D_8009B6EC = -1, D_8009B6F0 = 0, D_8009B6F8 = 0, D_8009B6F4
 * = a0, D_8009B6FC = a1, D_8009B700 = func_800824C8(func_80081E70),
 * D_8009B704 = func_800824DC(func_8008214C) (code addresses as the retail
 * VMA), D_8009B70C = 1; returns 1. */
/* func_80081D74: ported from the matching decomp -- generated TU pc_port/game/decomp/func_80081D74_port.c (src/func_80081D74.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* ── src/func_80083F44.c: card read step.  When D_8009B764 ==
 * D_8009B774 and D_8009B760 != 0 call the D_8009B754 then D_8009B750
 * hooks; with D_8009B7A4 set call the D_8009B73C hook on a0->+0xC and
 * a0->+0xC + 0xF0; then return func_800832B4(a0, a0[0x36] ? a0[0x36] :
 * 0x42).  None of the hook slots has a proven installed target and
 * func_800832B4 reads root counter 2 (0x1F801120), which pc_port does not
 * model: all are loud boundaries (the slot's loaded guest address is the
 * recorded vma). */
/* func_80083F44: ported from the matching decomp -- generated TU pc_port/game/decomp/func_80083F44_port.c (src/func_80083F44.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */
