/*
 * Hand adapters — batch 6 (>= 0x80060000).  Bodies follow the matched leaves
 * src/func_XXXXXXXX.c; pointers are guest addresses.  Callees with no
 * pc_port implementation stay loud boundaries (PE_D_COMP_BOUNDARYn).
 */
#include "pe_guest_decomp.h"
#include "pe_sdk.h"
#include "hand_hi_protos.h"

/* src/func_800C7DE4.c (+ twins 800C8F28 / 800CA7B8 with their own globals):
 * b = a0->+8; publish the three words +0x274/+0x278/+0x27C of the volatile
 * b->+0x238 record (re-read each time) as halfwords, store b, then
 * func_800CEDA8(0). */
static void publish_c238(pe_addr_t a0, pe_addr_t h0, pe_addr_t h1, pe_addr_t h2,
                         pe_addr_t slot)
{
    pe_addr_t b = PE_LoadU32(a0 + 8u);

    PE_StoreU16(h0, (uint16_t)PE_LoadU32(PE_LoadU32(b + 0x238u) + 0x274u));
    PE_StoreU16(h1, (uint16_t)PE_LoadU32(PE_LoadU32(b + 0x238u) + 0x278u));
    PE_StoreU16(h2, (uint16_t)PE_LoadU32(PE_LoadU32(b + 0x238u) + 0x27Cu));
    PE_StoreU32(slot, b);
    func_800CEDA8(0);
}
void func_800C7DE4(pe_addr_t a0)
{
    publish_c238(a0, 0x800E2348u, 0x800E234Au, 0x800E234Cu, 0x800E279Cu);
}
void func_800C8F28(pe_addr_t a0)
{
    publish_c238(a0, 0x800E2350u, 0x800E2352u, 0x800E2354u, 0x800E27A0u);
}
void func_800CA7B8(pe_addr_t a0)
{
    publish_c238(a0, 0x800E2360u, 0x800E2362u, 0x800E2364u, 0x800E27A8u);
}

/* src/func_800686A0.c: for i < p->+0x26 (u16), write (a1, a2, a3) bytes at
 * +4/+5/+6 of 16-byte entry i of the table at p->+0x30 and of its mirror
 * entry i + count. */
void func_800686A0(pe_addr_t a0, int a1, int a2, int a3)
{
    int i;

    for (i = 0; i < (int)PE_LoadU16(a0 + 0x26u); i++) {
        uint32_t off = (uint32_t)(i << 4);
        pe_addr_t q = PE_LoadU32(a0 + 0x30u) + off;

        PE_StoreU8(q + 4u, (uint8_t)a1);
        PE_StoreU8(q + 5u, (uint8_t)a2);
        PE_StoreU8(q + 6u, (uint8_t)a3);
        q = PE_LoadU32(a0 + 0x30u) + ((uint32_t)PE_LoadU16(a0 + 0x26u) << 4) + off;
        PE_StoreU8(q + 4u, (uint8_t)a1);
        PE_StoreU8(q + 5u, (uint8_t)a2);
        PE_StoreU8(q + 6u, (uint8_t)a3);
    }
}

/* src/func_80071034.c: D_8009D1A0 bit 1 -> func_800716A4() with the
 * caller's live $a0/$a1 (0x80071034: only v0/s0/s1/sp touched before the
 * `jal 0x800716a4`); otherwise when *a1 != 0x15: a0[26..28] = 0,
 * func_8001A680(a0, 0x15) (hand adapter, absent_lo_actor_port.c) and
 * *a1 = 0x15. */
void func_80071034(pe_addr_t a0, pe_addr_t a1)
{
    if ((PE_LoadU32(0x8009D1A0u) & 2u) != 0u) {
        func_800716A4(a0, a1);
    } else if ((int)PE_LoadU32(a1) != 0x15) {
        PE_StoreU32(a0 + 104u, 0u);
        PE_StoreU32(a0 + 108u, 0u);
        PE_StoreU32(a0 + 112u, 0u);
        func_8001A680(a0, 0x15u);
        PE_StoreU32(a1, 0x15u);
    }
}

/* src/func_8008A354.c: gate — a0 == 0 needs D_8009D2C8->+4 != 0, a0 != 0
 * must equal D_8009D2C8->+0x54 (u16).  Then D_8009D2C8->+0x18 = 0xFFFFFF
 * and 24 voice records (stride 0x11C) from a1 get +0x56 = 3, +0x58 = 1,
 * +0 = D_8009B8F4, +0xF4 |= 0x4400, +0x116 = 5. */
void func_8008A354(int a0, pe_addr_t a1)
{
    pe_addr_t rec = PE_LoadU32(0x8009D2C8u);
    unsigned int i;

    if (a0 == 0) {
        if (PE_LoadU32(rec + 4u) == 0u)
            return;
    } else if ((unsigned int)a0 != PE_LoadU16(rec + 0x54u)) {
        return;
    }
    PE_StoreU32(rec + 0x18u, 0xFFFFFFu);
    i = 0;
    do {
        i++;
        PE_StoreU16(a1 + 0x56u, 3u);
        PE_StoreU16(a1 + 0x58u, 1u);
        PE_StoreU32(a1, 0x8009B8F4u);
        PE_StoreU32(a1 + 0xF4u, PE_LoadU32(a1 + 0xF4u) | 0x4400u);
        PE_StoreU16(a1 + 0x116u, 5u);
        a1 += 0x11Cu;
    } while (i < 0x18u);
}

/* The D_8009D2C8 bank swap shared by src/func_8008C374.c / 8008C3E4.c /
 * 8008C55C.c / 8008C5D8.c / 8008C654.c: bank 0 runs on D_800B8AC0 with
 * the record as-is, bank 1 on D_800B8AC0 + 0x1AA0 with D_8009D2C8 advanced
 * by 0x68 (then restored). */
static void d2c8_shift(int delta)
{
    PE_StoreU32(0x8009D2C8u, PE_LoadU32(0x8009D2C8u) + (uint32_t)delta);
}

/* src/func_8008C374.c. */
void func_8008C374(void)
{
    func_8008A354(0, 0x800B8AC0u);
    d2c8_shift(0x68);
    func_8008A354(0, 0x800B8AC0u + 0x1AA0u);
    d2c8_shift(-0x68);
}

/* src/func_8008C3E4.c: bank 1 only when a0->+4 (re-read, volatile) != 0. */
void func_8008C3E4(pe_addr_t a0)
{
    func_8008A354((int)PE_LoadU32(a0 + 4u), 0x800B8AC0u);
    if (PE_LoadU32(a0 + 4u) != 0u) {
        int arg = (int)PE_LoadU32(a0 + 4u);

        d2c8_shift(0x68);
        func_8008A354(arg, 0x800B8AC0u + 0x1AA0u);
        d2c8_shift(-0x68);
    }
}

/* src/func_8008C55C.c / 8008C5D8.c / 8008C654.c: D_8009D2C0 = mode, mark
 * both banks (func_8008AB9C), then func_8008ABF0(). */
static void mark_banks(uint32_t mode)
{
    PE_StoreU32(0x8009D2C0u, mode);
    func_8008AB9C(0x800B8AC0u);
    d2c8_shift(0x68);
    func_8008AB9C(0x800B8AC0u + 0x1AA0u);
    d2c8_shift(-0x68);
    func_8008ABF0();
}
void func_8008C55C(void) { mark_banks(1u); }
void func_8008C5D8(void) { mark_banks(4u); }
void func_8008C654(void) { mark_banks(2u); }

/* src/func_800742B8.c: interrupt-state init at D_800945E4.  Already active
 * -> NULL.  Else func_800743A4(&s->f38) (+0x38; no pc_port implementation:
 * boundary), active = 1, *D_80095674 = mask (+0x32), *D_80095678 = +0x34,
 * func_80072724(), return s. */
pe_addr_t func_800742B8(void)
{
    const pe_addr_t s = 0x800945E4u;

    if (PE_LoadU16(s) != 0u)
        return 0u;
    PE_D_COMP_BOUNDARY1("func_800743A4", 0x800743A4u, s + 0x38u);
    PE_StoreU16(s, 1u);
    PE_StoreU16(PE_LoadU32(0x80095674u), PE_LoadU16(s + 0x32u));
    PE_StoreU32(PE_LoadU32(0x80095678u), PE_LoadU32(s + 0x34u));
    func_80072724();
    return s;
}

/* src/func_8006E590.c: w = *(a0 + *(a0 + 4) + 0x30); n = w >> 22 entries
 * of 12 bytes at a0 + (w & 0x3FFFFF); return a0 + (entry->+4 & 0xFFFFFF)
 * for the first entry whose +0xA halfword equals a1, else NULL. */
/* func_8006E590: ported from the matching decomp -- generated TU pc_port/game/decomp/func_8006E590_port.c (src/func_8006E590.c); hand port retired (port3 switch-over A). */

/* src/func_800C8BC0.c (+ byte-identical 800C9974 / 800CA4B4 / 800CBB30):
 * integrate position (+8/+A/+C) by velocity (+0x10/+0x12/+0x14), gravity
 * +3 on vy, ++frame (+1), bounce (vy = -vy) when y > 0, and a post-
 * decrement countdown at +2 that sets a1[1] = 2 when it was 0. */
static void particle_step(pe_addr_t a1, pe_addr_t a2)
{
    unsigned char t;

    PE_StoreU16(a2 + 8u, (uint16_t)(PE_LoadU16(a2 + 8u) + PE_LoadU16(a2 + 0x10u)));
    PE_StoreU16(a2 + 0xAu, (uint16_t)(PE_LoadU16(a2 + 0xAu) + PE_LoadU16(a2 + 0x12u)));
    PE_StoreU16(a2 + 0xCu, (uint16_t)(PE_LoadU16(a2 + 0xCu) + PE_LoadU16(a2 + 0x14u)));
    PE_StoreU16(a2 + 0x12u, (uint16_t)(PE_LoadU16(a2 + 0x12u) + 3u));
    PE_StoreU8(a2 + 1u, (uint8_t)(PE_LoadU8(a2 + 1u) + 1u));
    if ((int16_t)PE_LoadU16(a2 + 0xAu) > 0)
        PE_StoreU16(a2 + 0x12u, (uint16_t)(-(int16_t)PE_LoadU16(a2 + 0x12u)));
    t = PE_LoadU8(a2 + 2u);
    PE_StoreU8(a2 + 2u, (uint8_t)(t - 1u));
    if (t == 0)
        PE_StoreU8(a1 + 1u, 2u);
}
void func_800C8BC0(int a0, pe_addr_t a1, pe_addr_t a2) { (void)a0; particle_step(a1, a2); }
void func_800C9974(int a0, pe_addr_t a1, pe_addr_t a2) { (void)a0; particle_step(a1, a2); }
void func_800CA4B4(int a0, pe_addr_t a1, pe_addr_t a2) { (void)a0; particle_step(a1, a2); }
void func_800CBB30(int a0, pe_addr_t a1, pe_addr_t a2) { (void)a0; particle_step(a1, a2); }

/* src/func_800CD5EC.c: two SVECs at +0x10 advance by the two at +0x20
 * (x/y/z), then the signed frame byte +3 increments and a1[1] = 2 once it
 * reaches 8. */
void func_800CD5EC(int a0, pe_addr_t a1, pe_addr_t a2)
{
    unsigned short i;
    signed char c;

    (void)a0;
    for (i = 0; i < 2; i++) {
        pe_addr_t pos = a2 + 0x10u + i * 8u, vel = a2 + 0x20u + i * 8u;

        PE_StoreU16(pos + 0u, (uint16_t)(PE_LoadU16(pos + 0u) + PE_LoadU16(vel + 0u)));
        PE_StoreU16(pos + 2u, (uint16_t)(PE_LoadU16(pos + 2u) + PE_LoadU16(vel + 2u)));
        PE_StoreU16(pos + 4u, (uint16_t)(PE_LoadU16(pos + 4u) + PE_LoadU16(vel + 4u)));
    }
    c = (signed char)(PE_LoadU8(a2 + 3u) + 1);
    PE_StoreU8(a2 + 3u, (uint8_t)c);
    if (c >= 8)
        PE_StoreU8(a1 + 1u, 2u);
}
