/* 0x80083014 size 0x C8 ; -O2 -G0 + MASPSX_SYMBOL_AT_TEMP=1
 * 13 word mismatches, ALL of them the single-word shift caused by one unfilled
 * delay slot: retail emits
 *     lui $at,%hi(D_8009B768) / j .L800830CC / sw $v0,%lo(D_8009B768)($at)
 * i.e. patch 1's transformation but around a plain `j <label>`, not `j $31`.
 * Should become LINK_EXACT if MASPSX_FILL_STORE_DELAY_SLOT (or the new call
 * knob) is widened to an unconditional `j <label>`.
 */
extern int D_8009B768;
extern int (*D_8009B7A8[])();
extern void (*D_8009B724)(int a0);
extern void func_80084FC4(int a0);
extern int func_800834E8(void);

void func_80083014(void)
{
    register int *p asm("$5");
    register int i asm("$3");
    int (*f)();
    int r;
    int t;

    p = &D_8009B768;
    i = *p;
    f = D_8009B7A8[i];
    *p = i + 1;
    r = f();
    if (r < 0) {
        goto fail;
    }
    if (D_8009B768 != 0) {
        func_80084FC4(0x3C);
        if (func_800834E8() == 0) {
            D_8009B724(-3);
        }
    }
    t = D_8009B768;
    if (t >= 5) {
        D_8009B768 = t - 1;
    }
    return;
fail:
    D_8009B724(r);
}
