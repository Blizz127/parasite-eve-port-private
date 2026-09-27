/*
 * func_8007DCAC - libspu volatile x13 busy-wait (60 iterations).
 * vram 0x8007DCAC, size 0x5C (the three following words are nop pad, asm).
 * era: ERA_CC1_VER=2.8.1  cc1 -O2 -G0 -mno-split-addresses
 *      MASPSX_FILL_JUMP_STACK_STORE=1 (maspsx patch 11; 20 mismatches without)
 *
 * cc1 enters the loop with `j` to the test and leaves the volatile `i = 0`
 * store in front of it (reorg never fills a slot with a volatile insn);
 * retail's ASPSX moved that `sw $0,0($sp)` into the jump's delay slot.
 */
void func_8007DCAC(void)
{
    volatile int i;
    volatile int v;

    v = 13;
    for (i = 0; i < 60; i++) {
        v *= 13;
    }
}
