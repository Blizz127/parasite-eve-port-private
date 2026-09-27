/*
 * func_80077144 — GPU reset (ResetGraph-style): save the interrupt state
 * (func_80073E10(0) -> D_80095884), clear the command-queue cursors, then by
 * mode & 7: 0/5 = full reset (GP1 0x401, DPCR |= 0x800, clear GPU status,
 * wipe D_800A3348 and D_800BD030), 1/3 = soft reset (0x02000000 / 0x01000000
 * to GP1); restore the interrupt state; mode 0 also calls func_80077548(mode).
 *
 * ROM: ERA_CC1_VER=2.8.1 -O2 -G0 -mno-split-addresses. The hardware-register
 * pointer globals must be NON-volatile (a volatile store never goes in the
 * jal delay slot, 45 words); the D_80095874/78 cursors are volatile (retail
 * re-reads D_80095878 after zeroing it).
 */
extern unsigned int *D_80095854;
extern unsigned int *D_80095860;
extern unsigned int *D_80095870;
extern volatile int D_80095874;
extern volatile int D_80095878;
extern int D_80095884;
extern char D_800A3348;
extern char D_800BD030;
extern int func_80073E10(int);
extern void func_80077A28(void *, int, int);
extern int func_80077548(int);

int func_80077144(int mode) {
    D_80095884 = func_80073E10(0);
    D_80095878 = 0;
    D_80095874 = D_80095878;
    switch (mode & 7) {
    case 0:
    case 5:
        *D_80095860 = 0x401;
        *D_80095870 |= 0x800;
        *D_80095854 = 0;
        func_80077A28(&D_800A3348, 0, 0x100);
        func_80077A28(&D_800BD030, 0, 0x1800);
        break;
    case 1:
    case 3:
        *D_80095860 = 0x401;
        *D_80095870 |= 0x800;
        *D_80095854 = 0x2000000;
        *D_80095854 = 0x1000000;
        break;
    }
    func_80073E10(D_80095884);
    if (mode & 7) {
        return 0;
    }
    return func_80077548(mode);
}
