/*
 * func_80081D74 - vram 0x80081D74, size 0x84. libds dsread_control (DsReadCallback setup).
 * era: ERA_CC1_VER=2.8.1 -O2 -G0 -mno-split-addresses (profile era_o2_g0_cc1_281).
 * Explicit `one` local (retail li s1,1 used for the compare and the final store); an empty
 * register barrier just before `*p = one` stops CSE turning `return 1` into `move v0,s1`.
 */
extern int func_800824C8(void (*)());
extern int func_800824DC(void (*)());
extern void func_80081E70();
extern void func_8008214C();
extern int D_8009B70C;

int func_80081D74(int a0, int a1)
{
    int *p = &D_8009B70C;
    int one = 1;

    if (*p == one) {
        return 0;
    }
    p[-8] = -1;
    p[-7] = 0;
    p[-5] = 0;
    p[-6] = a0;
    p[-4] = a1;
    p[-3] = func_800824C8(func_80081E70);
    p[-2] = func_800824DC(func_8008214C);
    __asm__("" : "=r"(one) : "0"(one));
    *p = one;
    return 1;
}
