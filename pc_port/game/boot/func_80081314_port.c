/*
 * Phase 6E-B54K-AM/CDQ1 — complete func_80081314 (CdReadS stream open).
 *
 * Complete retail function: [0x80081314,0x800813E8), 53 words, SHA-256
 * fe43d63bd26dd2279dbdc1c8858998cace3f0da9d7cd16172aa61b8d32a6ce42.
 * Fully translated against asm/disc1/71B10.s: the mode&0x100 arm
 * installs the streaming callbacks (func_8007C214 via DMA slot 3,
 * func_800813E8 as the data callback), issues the 27-sector queue
 * through func_8007F0C8 with a -1 (streaming, no buffer) fifth arg —
 * that -1 is 81314's own sp+0x10 word, proven by the frame layout, not
 * assumed — and on nonzero issue returns the sequence with the
 * callbacks installed; on zero issue restores the previous callbacks
 * (824F0(old B8AB4), 824C8(0)) and returns 0.  The plain arm issues
 * directly and returns the queue result.
 */
#include "psx_compat.h"
#include "game_port.h"
#include "pe_sdk.h"
#include "pe_cdreg.h"
#include "pe_str_feed.h"

/* func_800824C8: ported from the matching decomp -- generated TU pc_port/game/decomp/func_800824C8_port.c (src/func_800824C8.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md). */

pe_addr_t func_800824F0(pe_addr_t callback)
{
    return func_80073CF4(3u, callback);
}

extern int func_8007F0C8(uint32_t mode, pe_addr_t loc, int count,
                         uint32_t a3, pe_addr_t buf);

int func_80081314(pe_addr_t location, uint32_t mode)
{
    if ((mode & 0x100u) != 0u) {
        pe_addr_t old_b8ab4;
        int status;

        PE_StoreU32(0x800A8020u, (mode & 0x20u) != 0u ? 0u : 1u);
        (void)func_800824F0(0x8007C214u);
        old_b8ab4 = func_800824C8(0x800813E8u);
        if (!PE_CdReg_DeviceEnabled() && PE_StrFeed_Probe(location)) {
            /* HOST_ADAPTED (fmv lane): with the CD register device
             * disabled nothing would ever drain the four SetMode/Setloc/
             * ReadS packets 7F0C8 queues (D_800A3608 += 4), which blocks
             * every later func_8006E6D4 read.  Hand the stream to the host
             * feeder and return a fresh command sequence instead. */
            uint32_t seq = PE_LoadU32(0x8009B53Cu) + 1u;
            if (seq == 0u)
                seq = 1u;
            PE_StoreU32(0x8009B53Cu, seq);
            PE_StrFeed_Open(location, mode);
            return (int)seq;
        }
        status = func_8007F0C8(mode & 0xFFu, location, 0x1Bu, 0u,
                               (pe_addr_t)0xFFFFFFFFu);
        if (status == 0) {
            (void)func_800824F0(old_b8ab4);
            (void)func_800824C8(0u);
            return 0;
        }
        return status;
    }
    return func_8007F0C8(mode & 0xFFu, location, 0x1Bu, 0u,
                         (pe_addr_t)0xFFFFFFFFu);
}
