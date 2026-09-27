/*
 * Hand adapters — batch 19 (>= 0x80060000).  Bodies follow the matched
 * leaves src/func_XXXXXXXX.c; pointers are guest addresses.
 */
#include "pe_guest_decomp.h"
#include "pe_sdk.h"
#include "hand_hi_protos.h"

/* Render-record fields used by src/func_800CBCA4.c:
 * s16 f0/f2/f4 @0/2/4; s32 f8/fC/f10 @8/0xC/0x10; u8 r/g/b @0x18..0x1A;
 * u8 f1C/f1D @0x1C/0x1D; s16 f1E @0x1E; s16 f28/f2A/f2C @0x28/0x2A/0x2C. */
static void rr_head(pe_addr_t r, int16_t f0, uint8_t f1C, uint8_t f1D, int16_t f1E)
{
    PE_StoreU16(r + 0u, (uint16_t)f0);
    PE_StoreU16(r + 2u, 0u);
    PE_StoreU16(r + 4u, 0u);
    PE_StoreU8(r + 0x1Cu, f1C);
    PE_StoreU8(r + 0x1Du, f1D);
    PE_StoreU16(r + 0x1Eu, (uint16_t)f1E);
}
static void rr_rgb(pe_addr_t r, uint8_t c)
{
    PE_StoreU8(r + 0x18u, c);
    PE_StoreU8(r + 0x19u, c);
    PE_StoreU8(r + 0x1Au, c);
}

/* src/func_800CBCA4.c: effect-slot reset (func_800C22F8 with the caller's
 * live $a0 — 0x800CBCB0 `jal 0x800c22f8` / `move s0,a0`), script
 * D_800E0D6C; D_800F3470 = a0[2] and its +0x2A/+0x2E/+0x32 position into
 * D_800E2268 +0x28/+0x2A/+0x2C; then the six render records' headers,
 * colours and D_800E2820's 0x1A8 scale words. */
int func_800CBCA4(pe_addr_t a0)
{
    pe_addr_t q;

    PE_StoreU32(func_800C22F8(a0), 0x800E0D6Cu);
    q = PE_LoadU32(a0 + 8u);
    PE_StoreU16(0x800E2268u + 0x28u, PE_LoadU16(q + 0x2Au));
    PE_StoreU32(0x800F3470u, q);
    PE_StoreU16(0x800E2268u + 0x2Au, PE_LoadU16(q + 0x2Eu));
    PE_StoreU16(0x800E2268u + 0x2Cu, PE_LoadU16(q + 0x32u));
    rr_head(0x800F3438u, 0x0, 0x68, 0x0, -0x64);
    rr_head(0x800E2268u, 0x400, 0x68, 0x0, 0x0);
    rr_head(0x800F32E8u, 0x400, 0x68, 0x0, -0x64);
    rr_rgb(0x800F32E8u, 0xA0);
    rr_head(0x800F3388u, 0x400, 0x46, 0x30, -0x50);
    rr_rgb(0x800F3388u, 0x80);
    PE_StoreU16(0x800E2820u + 0u, 0u);
    PE_StoreU16(0x800E2820u + 2u, 0u);
    PE_StoreU16(0x800E2820u + 4u, 0u);
    PE_StoreU32(0x800E2820u + 8u, 0x1A8u);
    PE_StoreU32(0x800E2820u + 0xCu, 0x1A8u);
    PE_StoreU32(0x800E2820u + 0x10u, 0x1A8u);
    PE_StoreU8(0x800E2820u + 0x1Cu, 0xBCu);
    PE_StoreU8(0x800E2820u + 0x1Du, 0x8u);
    PE_StoreU16(0x800E2820u + 0x1Eu, 0u);
    rr_rgb(0x800E2820u, 0x80);
    rr_head(0x800F3340u, 0x400, 0x42, 0x20, 0x32);
    rr_rgb(0x800F3340u, 0x80);
    return 0;
}

/* src/func_80065C38.c: when D_800BCF88 bit 6 is set, clamp the camera
 * target (a0, a1) into the current room's bounds — record D_800BCFFD (52
 * bytes each) of the table at D_800B1624 + D_800B1624->+0x1C: x in
 * [+0x2C, +0x2E] into D_800BCF8C, y in [+0x30, +0x32] into D_800BCF8E. */
int func_80065C38(int a0, int a1)
{
    if ((PE_LoadU32(0x800BCF88u) & 0x40u) != 0u) {
        pe_addr_t base = PE_LoadU32(0x800B1624u);
        pe_addr_t h = base + PE_LoadU32(base + 0x1Cu) + PE_LoadU8(0x800BCFFDu) * 52u;
        int x = (short)a0, y = (short)a1;
        int lo, hi;

        lo = (int16_t)PE_LoadU16(h + 0x2Cu);
        hi = (int16_t)PE_LoadU16(h + 0x2Eu);
        if (x < lo)
            PE_StoreU16(0x800BCF8Cu, (uint16_t)lo);
        else if (hi < x)
            PE_StoreU16(0x800BCF8Cu, (uint16_t)hi);
        else
            PE_StoreU16(0x800BCF8Cu, (uint16_t)a0);
        lo = (int16_t)PE_LoadU16(h + 0x30u);
        hi = (int16_t)PE_LoadU16(h + 0x32u);
        if (y < lo)
            PE_StoreU16(0x800BCF8Eu, (uint16_t)lo);
        else if (hi < y)
            PE_StoreU16(0x800BCF8Eu, (uint16_t)hi);
        else
            PE_StoreU16(0x800BCF8Eu, (uint16_t)a1);
    }
    return 0;
}

/* src/func_8007485C.c / 800748C0.c: LoadImage a 256- / 16-entry CLUT row
 * at (a1, a2) from a0 (RECT {a1, a2, w, 1} on the stack — passed to the
 * host LoadImage wrapper as a host RECT), then return its CLUT id
 * func_80077AA4(a1, a2). */
unsigned short func_8007485C(int a0, int a1, int a2)
{
    RECT r;

    r.x = (int16_t)a1; r.y = (int16_t)a2; r.w = 0x100; r.h = 1;
    (void)func_8007506C(&r, (pe_addr_t)a0);
    return (unsigned short)func_80077AA4(a1, (uint32_t)a2);
}
unsigned short func_800748C0(int a0, int a1, int a2)
{
    RECT r;

    r.x = (int16_t)a1; r.y = (int16_t)a2; r.w = 0x10; r.h = 1;
    (void)func_8007506C(&r, (pe_addr_t)a0);
    return (unsigned short)func_80077AA4(a1, (uint32_t)a2);
}
