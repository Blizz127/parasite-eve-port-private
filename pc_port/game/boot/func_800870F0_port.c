/*
 * Phase 6E-MV1a — func_800870F0 (42 words, 0x800870F0..0x80087198):
 * movie-frame scaler latch.  Pure byte stores, then a call to the
 * untranslated func_8007A88C (no asm in tree) — the CDQ1-anticipated
 * boundary.  Flag clear ([D_8009D2C0] & 2 == 0): [D1CA] = [D1C8] =
 * a0, [D1CB] = [D1C9] = 0.  Flag set: all four bytes hold
 * ((2903 * a0) >> 13) & 0xFF — the sll/addu/subu/srl chain is a
 * linear combination, so one 32-bit multiply-and-shift matches
 * retail exactly (wrapping included).  The beqz delay slot stores
 * $ra (executes on both arms); 7A88C's return is ignored (void).
 */

#include "psx_compat.h"
#include "game_port.h"
#include "pe_sdk.h"
#include "pe_bootstrap.h"
#include "pe_cdreg.h"

#define MV_D2C0 0x8009D2C0u
#define MV_D1C8 0x8009D1C8u
#define MV_D1C9 0x8009D1C9u
#define MV_D1CA 0x8009D1CAu
#define MV_D1CB 0x8009D1CBu

/* Phase 6E-MV1b — func_8007B964 (34 words, 0x8007B964..0x8007B9EC):
 * CD command-poke block.  Pushes the four latch bytes at p (the
 * 870F0 scaler output) plus the 2/3/0x20 tags through the CD
 * pointer tables ([B27C]/[B284]/[B288]/[B280]), shadow-routed by
 * CD0.  Returns 0 (addu in the jr delay slot); the sole caller
 * ignores it. */
void func_8007B964(pe_addr_t p)
{
    PE_CdStoreU8(PE_LoadU32(0x8009B27Cu), 2u);
    PE_CdStoreU8(PE_LoadU32(0x8009B284u), PE_LoadU8(p + 0u));
    PE_CdStoreU8(PE_LoadU32(0x8009B288u), PE_LoadU8(p + 1u));
    PE_CdStoreU8(PE_LoadU32(0x8009B27Cu), 3u);
    PE_CdStoreU8(PE_LoadU32(0x8009B280u), PE_LoadU8(p + 2u));
    PE_CdStoreU8(PE_LoadU32(0x8009B284u), PE_LoadU8(p + 3u));
    PE_CdStoreU8(PE_LoadU32(0x8009B288u), 0x20u);
}

/* func_8007A88C: ported from the matching decomp -- generated TU pc_port/game/decomp/func_8007A88C_port.c (src/func_8007A88C.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md). */

/* func_800870F0: ported from the matching decomp -- generated TU pc_port/game/decomp/func_800870F0_port.c (src/func_800870F0.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md). */
