/*
 * func_80083F44 - vram 0x80083F44, size 0xD8. memcard MemCard_WriteProtocol step.
 * era: ERA_CC1_VER=2.8.1 -O2 -G0 -mno-split-addresses (profile era_o2_g0_cc1_281).
 * Tail spelled as two calls, nonzero arm first: cross-jumping merges the jal, keeps the
 * a0[0x36] reload in the taken arm and `li a1,0x42` in the j slot (retail shape).
 */
extern int D_8009B764;
extern int D_8009B774;
extern int D_8009B760;
extern int D_8009B7A4;
extern void (*D_8009B754)(void);
extern void (*D_8009B750)(void);
extern void (*D_8009B73C)(int);
extern int func_800832B4(unsigned char *a0, int a1);

int func_80083F44(unsigned char *a0)
{
    if (D_8009B764 == D_8009B774 && D_8009B760 != 0) {
        D_8009B754();
        D_8009B750();
    }
    if (D_8009B7A4 != 0) {
        D_8009B73C(*(int *)(a0 + 0xC));
        D_8009B73C(*(int *)(a0 + 0xC) + 0xF0);
    }
    if (a0[0x36] != 0) {
        return func_800832B4(a0, a0[0x36]);
    }
    return func_800832B4(a0, 0x42);
}
