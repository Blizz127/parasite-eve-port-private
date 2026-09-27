/*
 * Hand adapters — port_absent lane round 2 (2026-09-24), SPU voice /
 * sequence leaves 0x80087000..0x8008FFFF.  Each body follows the matched
 * leaf src/func_XXXXXXXX.c named in its comment: guest pointers are
 * pe_addr_t, guest-layout struct fields are read at the leaf's offsets and
 * widths, retail stack locals handed to guest-address callees live at
 * PE_HAND_HI2_STACK (hand_absent_hi2_protos.h), and callees with no pc_port
 * implementation are loud boundaries.  Tests: pc_port/tests/test_pa_hi2.h.
 */
#include "pe_guest_decomp.h"
#include "pe_spu_dma.h"
#include "hand_absent_hi2_protos.h"

/* decomp-derived callees (generated TUs in pc_port/game/decomp/) */
void func_80089724(pe_addr_t pe_a0, pe_addr_t pe_a1, unsigned int a2, unsigned int a3);
void func_8008F1B0(pe_addr_t pe_a0, unsigned int a1);
void func_80089960(void);
void func_80089B28(void);
void func_80089CF0(void);
int func_80085BB4(unsigned int a0);
void func_8007DAE0(int a0, unsigned int a1, int a2);

/* ── src/func_800878F0.c ───────────────────────────────────────────────
 * Apply the pending voice-register updates in VoiceRegs a1 (guest layout:
 * +4 flags, +8/+0xC/+0x10/+0x14/+0x18 words, +0x1C..+0x26 u16, +0x28/+0x2A
 * s16) to voice a0, clearing each group's flag bits and returning as soon as
 * none remain; a final 0x9000 group writes +0x20/+0x22 and flags = 0.  The
 * tests use the value `fl` read on entry; each clear re-reads flags.  Field
 * widths: pe_dis.sh 0x800878F0 0x80 — `lhu a1,28(s0)` for f1C, `lh a1,40` /
 * `lh a2,42` for the signed f28/f2A pair. */
void func_800878F0(int a0, pe_addr_t a1)
{
    const pe_addr_t fp = a1 + 4u;
    unsigned int fl = PE_LoadU32(fp);

#define HI2_CLEAR(bits) \
    do { PE_StoreU32(fp, PE_LoadU32(fp) & ~(uint32_t)(bits)); \
         if (PE_LoadU32(fp) == 0) return; } while (0)

    if (fl == 0)
        return;
    if (fl & 0x10u) {
        func_800877BC(a0, PE_LoadU16(a1 + 0x1Cu));
        HI2_CLEAR(0x10u);
    }
    if (fl & 3u) {
        func_80087798(a0, (short)PE_LoadU16(a1 + 0x28u), (short)PE_LoadU16(a1 + 0x2Au));
        HI2_CLEAR(3u);
    }
    if (fl & 0x80u) {
        func_800877D4(a0, PE_LoadU32(a1 + 8u));
        HI2_CLEAR(0x80u);
    }
    if (fl & 0x10000u) {
        func_800877F0(a0, PE_LoadU32(a1 + 0xCu));
        HI2_CLEAR(0x10000u);
    }
    if (fl & 0x2200u) {
        func_8008788C(a0, PE_LoadU16(a1 + 0x24u), PE_LoadU32(a1 + 0x14u));
        HI2_CLEAR(0x2200u);
    }
    if (fl & 0x900u) {
        func_8008780C(a0, PE_LoadU16(a1 + 0x1Eu), PE_LoadU32(a1 + 0x10u));
        HI2_CLEAR(0x900u);
    }
    if (fl & 0x4400u) {
        func_800878C0(a0, PE_LoadU16(a1 + 0x26u), PE_LoadU32(a1 + 0x18u));
        HI2_CLEAR(0x4400u);
    }
#undef HI2_CLEAR
    if (fl & 0x9000u) {
        func_8008783C(a0, PE_LoadU16(a1 + 0x20u));
        func_80087864((unsigned int)a0, PE_LoadU16(a1 + 0x22u));
    }
    PE_StoreU32(fp, 0u);
}

/* ── src/func_8008900C.c ───────────────────────────────────────────────
 * For every set bit of a1 (voice records a0[i], stride 0x11C): run
 * func_80088344(a0, bit, i) (no pc_port implementation: loud boundary with
 * the guest record address); if the record's +0xF4 flags are live and the
 * bit is in a1 & D_8009D2C8->+0x10, either take voice i (a2 bit set) or
 * claim the first free D_800B002C slot (stride 8, 0x7FFF marks it taken,
 * D_8009D2C4 |= 0x100) — with none free, +0xF0 = 24 and *D_8009D2C8 |= 1.
 * Claimed voices set *a3 bit and +0xF4 |= 0x1FF93.  A D_8009D2B8 bit clears
 * +0x118/+0x11A; a voice < 24 is programmed via func_800878F0(+0xF0,
 * &+0xF0) (the matched C's third argument, +0x38, is not a parameter of
 * func_800878F0 and is dropped). */
void func_8008900C(pe_addr_t a0, unsigned int a1, unsigned int a2, pe_addr_t a3)
{
    unsigned int bit = 1;
    unsigned int i = 0;
    unsigned int j;
    unsigned int m;
    pe_addr_t q;

    m = a1 & PE_LoadU32(PE_LoadU32(0x8009D2C8u) + 0x10u);
    do {
        if (a1 & bit) {
            PE_D_COMP_BOUNDARY3("func_80088344", 0x80088344u, a0, bit, i);
            if (PE_LoadU32(a0 + 0xF4u) != 0) {
                if (m & bit) {
                    if (a2 & bit) {
                        PE_StoreU32(a3, PE_LoadU32(a3) | (1u << i));
                        PE_StoreU32(a0 + 0xF0u, i);
                        PE_StoreU32(a0 + 0xF4u, PE_LoadU32(a0 + 0xF4u) | 0x1FF93u);
                    } else {
                        j = 0;
                        q = 0x800B002Cu;
                        while (j < 24) {
                            if (PE_LoadU16(q) == 0) {
                                PE_StoreU32(a0 + 0xF4u, PE_LoadU32(a0 + 0xF4u) | 0x1FF93u);
                                PE_StoreU32(a3, PE_LoadU32(a3) | (1u << j));
                                PE_StoreU32(a0 + 0xF0u, j);
                                PE_StoreU16(q, 0x7FFFu);
                                PE_StoreU32(0x8009D2C4u, PE_LoadU32(0x8009D2C4u) | 0x100u);
                                j = 24;
                            } else {
                                j++;
                                q += 8u;
                                if (j == 24) {
                                    pe_addr_t t = PE_LoadU32(0x8009D2C8u);
                                    PE_StoreU32(a0 + 0xF0u, j);
                                    PE_StoreU32(t, PE_LoadU32(t) | 1u);
                                }
                            }
                        }
                    }
                }
                if (PE_LoadU32(0x8009D2B8u) & bit) {
                    PE_StoreU16(a0 + 0x11Au, 0);
                    PE_StoreU16(a0 + 0x118u, 0);
                }
                if (PE_LoadU32(a0 + 0xF0u) < 24)
                    func_800878F0((int)PE_LoadU32(a0 + 0xF0u), a0 + 0xF0u);
            }
            a1 &= ~bit;
        }
        bit <<= 1;
        a0 += 0x11Cu;
        i++;
    } while (a1 != 0);
}

/* ── src/func_80089784.c (+ 80089980 / 80089B48 / 80089D10) ────────────
 * Collect the voices to key off (func_80089784) or to update
 * (func_80089980 / 80089B48 / 80089D10) across the two track banks: bank 1
 * is D_8009D2C8 + 0x68 over records D_800BA560, bank 0 is D_8009D2C8 over
 * D_800B8AC0.  func_80089724 ORs `1 << record+0xF0` into the stack word
 * `acc` (PE_HAND_HI2_STACK + 0) for each selected record, then masks it
 * with ~(D_800BCD50 | D_800BCD60).  D_8009D2C8 is advanced by 0x68 around
 * each bank-1 call exactly as the leaf does, and every field read reloads
 * the global. */
#define HI2_TRK  0x8009D2C8u
#define HI2_ACC  (PE_HAND_HI2_STACK + 0x00u)

static pe_addr_t hi2_trk(void) { return PE_LoadU32(HI2_TRK); }
static void hi2_trk_add(int32_t d) { PE_StoreU32(HI2_TRK, hi2_trk() + (uint32_t)d); }

void func_80089784(void)
{
    pe_addr_t p = hi2_trk();
    pe_addr_t q;
    unsigned int mask;
    unsigned int s1v;
    unsigned int s0v;

    PE_StoreU32(HI2_ACC, 0u);
    mask = ~(PE_LoadU32(0x800BCD50u) | PE_LoadU32(0x800BCD60u));
    s1v = PE_LoadU32(p + 0x6Cu) & PE_LoadU32(p + 0x80u);
    if (s1v & PE_LoadU32(p + 0x70u)) {
        PE_StoreU32(HI2_TRK, p + 0x68u);
        func_80089724(0x800BA560u, HI2_ACC, s1v & PE_LoadU32(p + 0x70u), mask);
        q = hi2_trk();
        s1v &= ~PE_LoadU32(q + 8u);
        PE_StoreU32(HI2_TRK, q - 0x68u);
        PE_StoreU32(q + 0x18u, PE_LoadU32(q + 0x18u) & ~PE_LoadU32(q + 8u));
    }
    s0v = PE_LoadU32(hi2_trk() + 4u) & PE_LoadU32(hi2_trk() + 0x18u);
    if (s0v & PE_LoadU32(hi2_trk() + 8u)) {
        func_80089724(0x800B8AC0u, HI2_ACC, s0v & PE_LoadU32(hi2_trk() + 8u), mask);
        q = hi2_trk();
        PE_StoreU32(q + 0x18u, PE_LoadU32(q + 0x18u) & ~PE_LoadU32(q + 8u));
        s0v &= ~PE_LoadU32(q + 8u);
    }
    if (s1v != 0) {
        hi2_trk_add(0x68);
        func_80089724(0x800BA560u, HI2_ACC, s1v, mask);
        PE_StoreU32(hi2_trk() + 0x18u, 0u);
        hi2_trk_add(-0x68);
    }
    if (s0v != 0) {
        func_80089724(0x800B8AC0u, HI2_ACC, s0v, mask);
        PE_StoreU32(hi2_trk() + 0x18u, 0u);
    }
    PE_StoreU32(HI2_ACC, PE_LoadU32(HI2_ACC) | PE_LoadU32(0x800BCD5Cu));
    PE_StoreU32(0x800BCD5Cu, 0u);
    if (PE_LoadU32(HI2_ACC) != 0)
        func_80087728(PE_LoadU32(HI2_ACC));
}

/* The three update collectors differ only in the bank-1 select word
 * (+0x9C / +0xA0 / +0xA4), the bank-0 select word (+0x34 / +0x38 / +0x3C),
 * the extra mask OR-ed in and the destination word. */
static void hi2_collect(uint32_t sel1, uint32_t sel0, pe_addr_t extra, pe_addr_t dest)
{
    pe_addr_t p = hi2_trk();
    unsigned int mask;
    unsigned int s1v;
    unsigned int s0v;

    PE_StoreU32(HI2_ACC, 0u);
    mask = ~(PE_LoadU32(0x800BCD50u) | PE_LoadU32(0x800BCD60u));
    s1v = PE_LoadU32(p + 0x6Cu) & PE_LoadU32(p + sel1);
    if (s1v & PE_LoadU32(p + 0x70u)) {
        PE_StoreU32(HI2_TRK, p + 0x68u);
        func_80089724(0x800BA560u, HI2_ACC, s1v & PE_LoadU32(p + 0x70u), mask);
        s1v &= ~PE_LoadU32(hi2_trk() + 8u);
        hi2_trk_add(-0x68);
    }
    s0v = PE_LoadU32(hi2_trk() + 4u) & PE_LoadU32(hi2_trk() + sel0);
    if (s0v & PE_LoadU32(hi2_trk() + 8u)) {
        func_80089724(0x800B8AC0u, HI2_ACC, s0v & PE_LoadU32(hi2_trk() + 8u), mask);
        s0v &= ~PE_LoadU32(hi2_trk() + 8u);
    }
    if (s1v != 0) {
        hi2_trk_add(0x68);
        func_80089724(0x800BA560u, HI2_ACC, s1v, mask);
        hi2_trk_add(-0x68);
    }
    if (s0v != 0)
        func_80089724(0x800B8AC0u, HI2_ACC, s0v, mask);
    PE_StoreU32(dest, PE_LoadU32(HI2_ACC) | PE_LoadU32(extra));
    PE_StoreU32(0x8009D2C4u, PE_LoadU32(0x8009D2C4u) | 0x100u);
}

/* src/func_80089980.c: selects +0x9C / +0x34; D_800C0DD4 = acc | D_800BCD6C. */
void func_80089980(void) { hi2_collect(0x9Cu, 0x34u, 0x800BCD6Cu, 0x800C0DD4u); }
/* src/func_80089B48.c: selects +0xA0 / +0x38; D_800C0DD0 = acc | D_800BCD70. */
void func_80089B48(void) { hi2_collect(0xA0u, 0x38u, 0x800BCD70u, 0x800C0DD0u); }
/* src/func_80089D10.c: selects +0xA4 / +0x3C; D_800C0DD8 = acc | D_800BCD74. */
void func_80089D10(void) { hi2_collect(0xA4u, 0x3Cu, 0x800BCD74u, 0x800C0DD8u); }

/* ── src/func_8008A400.c ───────────────────────────────────────────────
 * Stop sequence id a0 (low 16 bits; 0xFFFF = none) across the 12 records
 * of D_800BC000 (stride 0x11C, bit 0x1000 << i in D_800BCD50):
 *   a1 & 0x0FFFFFFF  — records whose +0x2C & a1;
 *   a1 < 0           — recurse on the +0x28 ids of records id and id + 1
 *                      (when active) and return without the tail calls;
 *   a1 & 0x40000000  — among active records with +0x2C == 0, those whose
 *                      +0x50 equals the maximum (signed, floor 0);
 *   otherwise        — active records whose +0x28 == id.
 * A selected record with +0x38 bit 0x100000 gets bit 0x200000; otherwise
 * D_800BCD5C |= its bit, func_8008F1B0(record, bit), +0x38 = 0.  Then
 * D_8009D2C4 |= 0x10 and func_80089960 / 80089B28 / 80089CF0. */
static void hi2_stop(pe_addr_t base, unsigned int mask)
{
    if (PE_LoadU32(base + 0x38u) & 0x100000u) {
        PE_StoreU32(base + 0x38u, PE_LoadU32(base + 0x38u) | 0x200000u);
    } else {
        PE_StoreU32(0x800BCD5Cu, PE_LoadU32(0x800BCD5Cu) | mask);
        func_8008F1B0(base, mask);
        PE_StoreU32(base + 0x38u, 0u);
    }
}

void func_8008A400(unsigned int a0, unsigned int a1)
{
    pe_addr_t base;
    unsigned int mask = 0x1000;
    unsigned int act;
    unsigned int i;
    int max;
    unsigned int id = a0 & 0xFFFFu;

    if (id == 0xFFFF)
        return;
    base = 0x800BC000u;
    act = PE_LoadU32(0x800BCD50u);
    if (a1 & 0x0FFFFFFFu) {
        for (i = 0; i < 12; i++, base += 0x11Cu, mask <<= 1)
            if ((act & mask) && (PE_LoadU32(base + 0x2Cu) & a1))
                hi2_stop(base, mask);
    } else if ((int)a1 < 0) {
        base += id * 0x11Cu;
        mask <<= id;
        if (act & mask)
            func_8008A400(PE_LoadU32(base + 0x28u), 0);
        mask <<= 1;
        base += 0x11Cu;
        if (act & mask)
            func_8008A400(PE_LoadU32(base + 0x28u), 0);
        return;
    } else if (a1 & 0x40000000u) {
        for (i = 0; i < 12; i++, base += 0x11Cu, mask <<= 1)
            if (PE_LoadU32(base + 0x2Cu))
                act &= ~mask;
        base = 0x800BC000u;
        mask = 0x1000;
        max = 0;
        for (i = 0; i < 12; i++, base += 0x11Cu, mask <<= 1)
            if ((act & mask) && max < (int)PE_LoadU32(base + 0x50u))
                max = (int)PE_LoadU32(base + 0x50u);
        base = 0x800BC000u;
        mask = 0x1000;
        for (i = 0; i < 12; i++, base += 0x11Cu, mask <<= 1)
            if ((act & mask) && max == (int)PE_LoadU32(base + 0x50u))
                hi2_stop(base, mask);
    } else {
        for (i = 0; i < 12; i++, base += 0x11Cu, mask <<= 1)
            if ((act & mask) && PE_LoadU32(base + 0x28u) == id)
                hi2_stop(base, mask);
    }
    PE_StoreU32(0x8009D2C4u, PE_LoadU32(0x8009D2C4u) | 0x10u);
    func_80089960();
    func_80089B28();
    func_80089CF0();
}

/* ── src/func_8008CF70.c ───────────────────────────────────────────────
 * Select reverb preset a0 (bit 0x100 = also clear the work area): reject
 * a0 >= 10 or a failing func_80085BB4(D_8009B46C[a0]) with -1.  Copy the
 * 68-byte preset D_8009C8C0 + a0 * 68 into the stack RevPreset `entry`
 * (PE_HAND_HI2_STACK + 0x10) and zero its mask word; D_8009B39C.mode = a0,
 * D_8009B398 = start address; delay/feedback = 0x7F/0x7F (7), 0x7F/0 (8),
 * else 0/0.  With SPUCNT (D_8009B3FC[0xD5], +0x1AA) bit 7 set it is
 * cleared around the update; reverb depth registers +0x184/+0x186 and the
 * RevAttr depths are zeroed; func_8008D140(&entry); func_8008D610(a0) when
 * clearing (no pc_port implementation: loud boundary); func_8007DAE0(0xD1,
 * D_8009B398, 0).  D_8009B3FC holds the SPU register base; register
 * accesses go through the pc_port SPU register model exactly as
 * func_8007D614 (absent_cd_port.c) does — a RAM base stays guest RAM. */
static uint16_t hi2_spu_load(uint32_t off)
{
    pe_addr_t regs = PE_LoadU32(0x8009B3FCu);

    if ((regs & 0x1FFFFFFFu) == 0x1F801C00u)
        return PE_SpuRegister_LoadU16(off);
    return PE_LoadU16(regs + off);
}

static void hi2_spu_store(uint32_t off, uint16_t v)
{
    pe_addr_t regs = PE_LoadU32(0x8009B3FCu);

    if ((regs & 0x1FFFFFFFu) == 0x1F801C00u)
        PE_SpuRegister_StoreU16(off, v);
    else
        PE_StoreU16(regs + off, v);
}

int func_8008CF70(int a0)
{
    const pe_addr_t entry = PE_HAND_HI2_STACK + 0x10u;
    pe_addr_t p;
    pe_addr_t q;
    int clear = 0;
    int on;
    int n;
    uint32_t sa;

    if (a0 & 0x100) {
        a0 &= ~0x100;
        clear = 1;
    }
    if ((uint32_t)a0 >= 10)
        return -1;
    sa = PE_LoadU32(0x8009B46Cu + (uint32_t)a0 * 4u);
    if (func_80085BB4(sa) != 0)
        return -1;
    q = entry;
    p = 0x8009C8C0u + (uint32_t)a0 * 68u;
    PE_StoreU32(0x8009B39Cu + 4u, (uint32_t)a0);
    PE_StoreU32(0x8009B398u, sa);
    n = 67;
    do {
        PE_StoreU8(q++, PE_LoadU8(p++));
    } while (--n != -1);
    PE_StoreU32(entry, 0u);
    switch (a0) {
    case 7:
        PE_StoreU32(0x8009B39Cu + 0x10u, 0x7Fu);
        PE_StoreU32(0x8009B39Cu + 0xCu, 0x7Fu);
        break;
    case 8:
        PE_StoreU32(0x8009B39Cu + 0x10u, 0u);
        PE_StoreU32(0x8009B39Cu + 0xCu, 0x7Fu);
        break;
    default:
        PE_StoreU32(0x8009B39Cu + 0x10u, 0u);
        PE_StoreU32(0x8009B39Cu + 0xCu, 0u);
        break;
    }
    on = (hi2_spu_load(0xD5u * 2u) & 0x80u) != 0;
    if (on)
        hi2_spu_store(0xD5u * 2u, (uint16_t)(hi2_spu_load(0xD5u * 2u) & 0xFF7Fu));
    hi2_spu_store(0xC2u * 2u, 0u);
    hi2_spu_store(0xC3u * 2u, 0u);
    PE_StoreU16(0x8009B39Cu + 8u, 0u);
    PE_StoreU16(0x8009B39Cu + 0xAu, 0u);
    func_8008D140(entry);
    if (clear)
        PE_D_COMP_BOUNDARY1("func_8008D610", 0x8008D610u, a0);
    func_8007DAE0(0xD1, PE_LoadU32(0x8009B398u), 0);
    if (on)
        hi2_spu_store(0xD5u * 2u, (uint16_t)(hi2_spu_load(0xD5u * 2u) | 0x80u));
    return 0;
}
