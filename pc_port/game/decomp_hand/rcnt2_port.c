/*
 * func_80084FC4 / func_80084FE4 — root-counter-2 timeout (start / elapsed),
 * from the matched src/func_80084FC4.c and src/func_80084FE4.c.  The
 * counter/mode/target registers (0x1F801120/24/28) are the pe_rcnt2.h model;
 * every retail register read is one model read (pe_dis.sh 0x80084FE4 0x74:
 * count once, target twice — test then add —, mode once; a mode read clears
 * the reached flags as on hardware).
 *
 * Limitation (documented, not invented): the model advances on the port's
 * device-time ticks (HostFB_DeviceTime), not per polled instruction, so a
 * timeout that retail reaches *inside* a tight poll loop does not elapse
 * until the next device tick — the same observable result as the loud
 * boundary these calls replaced (func_80084FE4 returning 0).
 */
#include "pe_guest_decomp.h"
#include "pe_rcnt2.h"

/* D_800BD02C = limit; D_800A76D0 = the 16-bit count (zero-extended). */
void func_80084FC4(unsigned int limit)
{
    unsigned short count = PE_Rcnt2_ReadCounter();

    PE_StoreU32(0x800BD02Cu, limit);
    PE_StoreU32(0x800A76D0u, count);
}

/* Elapsed test: t = count, plus one period (target, or 0x10000 when the
 * target is 0) when the counter wrapped below the base; counter-2 mode bit 9
 * clear (system clock) scales by 1/8; returns (t - base) >= limit
 * (unsigned). */
int func_80084FE4(void)
{
    unsigned int t = PE_Rcnt2_ReadCounter();
    unsigned int d;

    if (t < PE_LoadU32(0x800A76D0u)) {
        if (PE_Rcnt2_ReadTarget() != 0)
            t = t + PE_Rcnt2_ReadTarget();
        else
            t = t + 0x10000u;
    }
    if ((PE_Rcnt2_ReadMode() & 0x200u) == 0)
        d = (t - PE_LoadU32(0x800A76D0u)) >> 3;
    else
        d = t - PE_LoadU32(0x800A76D0u);
    return (d < PE_LoadU32(0x800BD02Cu)) ^ 1;
}
