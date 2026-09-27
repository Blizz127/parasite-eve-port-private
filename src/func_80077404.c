/*
 * func_80077404 - vram 0x80077404, size 0x144. Draft by lane4.
 * era: ERA_CC1_VER=2.8.1  cc1 -O2 -G0 -mno-split-addresses
 *      MASPSX_UNFILL_EPILOGUE_DELAY_SLOT=1 (maspsx patch 15; 2 words without it:
 *      retail's ra-only epilogue is `lw $ra` / `addiu $sp` / `jr $ra` / nop).
 */
extern volatile unsigned int *D_80095854;
extern volatile unsigned int *D_80095858;
extern volatile unsigned int *D_80095860;
extern volatile unsigned int *D_80095870;
extern volatile int D_80095874;
extern volatile int D_80095878;
extern int D_80095884;
extern int D_80095888;
extern int D_8009588C;
extern char D_80011988;
extern int func_80073A44(int);
extern int func_80073E10(int);
extern void func_80071A74();

int func_80077404(void) {
    if (func_80073A44(-1) > D_80095888 || D_8009588C++ > 0xF0000) {
        *D_80095854;
        func_80071A74(&D_80011988, (D_80095874 - D_80095878) & 0x3F, *D_80095854, *D_80095860, *D_80095858);
        D_80095884 = func_80073E10(0);
        D_80095878 = 0;
        D_80095874 = D_80095878;
        *D_80095860 = 0x401;
        *D_80095870 |= 0x800;
        *D_80095854 = 0x2000000;
        *D_80095854 = 0x1000000;
        func_80073E10(D_80095884);
        return -1;
    }
    return 0;
}
