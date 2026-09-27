/*
 * func_80083578 - vram 0x80083578. Draft by lane5.
 * era: ERA_CC1_VER=2.8.1  cc1 -O2 -G0 -mno-split-addresses
 *      MASPSX_LOAD_DELAY_NOP_BEFORE_LABEL=1 (maspsx patch 14; 1 word without it:
 *      retail has the load-delay nop before the volatile poll-loop label).
 */
extern volatile unsigned short *D_8009B788;

void func_80083578(void)
{
    unsigned short v; do { v = D_8009B788[2]; } while (!(v & 2));
}
