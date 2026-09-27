/*
 * Hand adapters — batch 20 (>= 0x80060000).  Bodies follow the matched
 * leaves src/func_XXXXXXXX.c; pointers are guest addresses.
 */
#include "pe_guest_decomp.h"
#include "pe_sdk.h"
#include "pe_spu_dma.h"
#include "hand_hi_protos.h"

extern unsigned int func_80071A54(void);
extern int32_t func_80077CF4(int32_t angle);
extern int32_t func_80077DC4(int32_t angle);
extern int func_80085BB4(unsigned int a0);

/* src/func_800D6E3C.c: debris fountain.  Mode 0 seeds *a1 with rand and
 * allocates 0x10 x 0x18 particles with callback func_800D6C58.  Mode 1
 * (frames < 8) spawns three per frame at the owner's position
 * (D_800F32D0->+8 -> +0x268/+0x26A/+0x26C) with a random fan direction
 * (*a1 += 0x955 + (r & 0x1F), speed (r & 0x7F) + 0x46), vy -(r & 0x1F) -
 * 0x24, life (r & 7) + 0x1A and a random spin; ends at 0x28.  Mode 2 sets
 * the draw state (texpage for stage D_800E11E4, func_800CEDA8(0)). */
int func_800D6E3C(int a0, pe_addr_t a1)
{
    switch (a0) {
    case 0: {
        int t = (int)func_80071A54();
        pe_addr_t q = PE_LoadU32(0x800F33E0u);

        PE_StoreU32(a1, (uint32_t)t);
        return (int)func_800CE560(PE_LoadU32(q + 8u), 0x10u, 0x18, 0x800D6C58u);
    }
    case 1:
        if ((int)PE_LoadU32(0x800E27ECu) < 8) {
            int i;

            for (i = 0; i < 3; i++) {
                pe_addr_t p = func_800CE610(PE_LoadU32(PE_LoadU32(0x800F33E0u) + 8u));

                if (p != 0u) {
                    pe_addr_t o = PE_LoadU32(PE_LoadU32(0x800F32D0u) + 8u);
                    int s1v;

                    PE_StoreU16(p + 0u, PE_LoadU16(o + 0x268u));
                    PE_StoreU16(p + 2u, PE_LoadU16(o + 0x26Au));
                    PE_StoreU16(p + 4u, PE_LoadU16(o + 0x26Cu));
                    PE_StoreU32(a1, PE_LoadU32(a1) + 0x955u + (func_80071A54() & 0x1Fu));
                    s1v = (int)(func_80071A54() & 0x7Fu) + 0x46;
                    PE_StoreU16(p + 6u, (uint16_t)(func_80077CF4((int)PE_LoadU32(a1)) * s1v / 4096));
                    PE_StoreU16(p + 10u, (uint16_t)(func_80077DC4((int)PE_LoadU32(a1)) * s1v / 4096));
                    PE_StoreU16(p + 8u, (uint16_t)(-(int)(func_80071A54() & 0x1Fu) - 0x24));
                    PE_StoreU16(p + 12u, (uint16_t)((func_80071A54() & 7u) + 0x1Au));
                    PE_StoreU16(p + 14u, (uint16_t)func_80071A54());
                }
            }
        }
        if ((int)PE_LoadU32(0x800E27ECu) < 0x28)
            break;
        return 1;
    case 2: {
        unsigned int ix = PE_LoadU16(0x800E11E4u);

        PE_StoreU16(0x800F3368u, 0x10u);
        PE_StoreU16(0x800F336Au, 1u);
        PE_StoreU16(0x800F3376u, 0x10u);
        PE_StoreU16(0x800F3378u, 0x10u);
        PE_StoreU16(0x800F336Cu, 0u);
        PE_StoreU16(0x800F3370u, PE_LoadU16(0x800E2850u + ix * 2u));
        func_800CEDA8(0);
        PE_StoreU16(0x800F336Eu, 0u);
        PE_StoreU16(0x800F3372u, 0u);
        PE_StoreU16(0x800F3374u, 0u);
        break;
    }
    default:
        return 0;
    }
    return 0;
}

/* The SPU control halfword D_8009B3FC[0xD5] (+0x1AA), routed like
 * pe_stream.c's stream_register_write when the base is the SPU window. */
static uint16_t spucnt_load(void)
{
    pe_addr_t base = PE_LoadU32(0x8009B3FCu);

    return ((base & 0x1FFFFFFFu) == 0x1F801C00u) ? PE_SpuRegister_LoadU16(0x1AAu)
                                                 : PE_LoadU16(base + 0x1AAu);
}
static void spucnt_store(uint16_t v)
{
    pe_addr_t base = PE_LoadU32(0x8009B3FCu);

    if ((base & 0x1FFFFFFFu) == 0x1F801C00u)
        PE_SpuRegister_StoreU16(0x1AAu, v);
    else
        PE_StoreU16(base + 0x1AAu, v);
}

/* src/func_80085A64.c: reverb enable.  0 clears D_8009B390 and SPUCNT bit
 * 7; 1 sets both unless reverb is not yet configured (D_8009B394 != 1)
 * and the work area D_8009B398 is already allocated (func_80085BB4), in
 * which case it clears.  Returns D_8009B390. */
int func_80085A64(int a0)
{
    switch (a0) {
    case 0:
        PE_StoreU32(0x8009B390u, 0u);
        spucnt_store((uint16_t)(spucnt_load() & 0xFF7Fu));
        break;
    case 1:
        if ((int)PE_LoadU32(0x8009B394u) != a0 &&
            func_80085BB4(PE_LoadU32(0x8009B398u)) != 0) {
            PE_StoreU32(0x8009B390u, 0u);
            spucnt_store((uint16_t)(spucnt_load() & 0xFF7Fu));
        } else {
            PE_StoreU32(0x8009B390u, (uint32_t)a0);
            spucnt_store((uint16_t)(spucnt_load() | 0x80u));
        }
        break;
    }
    return (int)PE_LoadU32(0x8009B390u);
}

/* src/func_8007BBB0.c: clear the four CD callback words, DMA-callback reset
 * (func_80073C94), then install func_8007C13C (retail text address) as the
 * DMA channel-3 handler through func_80073CC4(2, ·). */
void func_8007BBB0(void)
{
    PE_StoreU32(0x8009AFB8u, 0u);
    PE_StoreU32(0x8009AFB4u, 0u);
    PE_StoreU32(0x8009AFC8u, 0u);
    PE_StoreU32(0x8009AFC4u, 0u);
    func_80073C94();
    (void)func_80073CC4(2u, 0x8007C13Cu);
}
