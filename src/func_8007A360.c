/*
 * func_8007A360 - vram 0x8007A360, size 0x6C. libcd CdReset.
 * era: ERA_CC1_VER=2.8.1 -O2 -G0 -mno-split-addresses (profile era_o2_g0_cc1_281).
 * Note the a0 != 1 path returns 1 (retail reuses the li v0,1 compare constant as the result).
 */
extern void func_8007BBB0(void);
extern int func_8007BBFC(void);
extern int func_8007BAC0(void);

int func_8007A360(int a0)
{
    if (a0 == 2) {
        func_8007BBB0();
        return 1;
    }
    if (func_8007BBFC() != 0) {
        return 0;
    }
    if (a0 == 1 && func_8007BAC0() != 0) {
        return 0;
    }
    return 1;
}
