/* VRAM 0x80076354 / file 0x66B54 / size 0xE0.
 *
 * DMA channel-2 ordering-table kick (DPCR enable, MADR = &ot[n-1], BCR = n,
 * CHCR = 0x11000002), then poll CHCR busy with the func_80077404 timeout.
 * era -O2 -G0 (2.7.2) + maspsx patch 18 (MASPSX_FILL_CALL_VOLATILE_STORE=1:
 * retail 0x800763C4 `jal func_800773D0` / `sw $v1,0($v0)` -- the volatile
 * CHCR store cc1 cannot slot) + patch 3 (MASPSX_FILL_EPILOGUE_DELAY_SLOT=1,
 * 0x8007642C `jr $ra` / `addiu $sp,$sp,32`). Without patch 18: 28 words;
 * without patch 3: 2 words. The 2.8.1 rung is worse (20 words: s0/s1 swap
 * from a CSE'd loop mask).
 */
extern volatile unsigned int *D_80095864;
extern volatile unsigned int *D_80095868;
extern volatile unsigned int *D_8009586C;
extern volatile unsigned int *D_80095870;
extern void func_800773D0(void);
extern int func_80077404(void);

int func_80076354(unsigned int *ot, int n) {
    *D_80095870 |= 0x8000000;
    *D_8009586C = 0;
    *D_80095864 = (unsigned int)&ot[n - 1];
    *D_80095868 = n;
    *D_8009586C = 0x11000002;
    func_800773D0();
    while (*D_8009586C & 0x1000000) {
        if (func_80077404()) {
            return -1;
        }
    }
    return n;
}
