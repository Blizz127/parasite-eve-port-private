extern int D_8009B700[4];
extern void func_800824C8(int a0);
extern void func_800824DC(int a0);
extern void func_8007EE84(int a0, int a1, int a2, int a3);

void func_80081DF8(void)
{
    int *p;

    p = &D_8009B700[3];
    asm volatile("" : "=r" (p) : "0" (p));
    if (*p == 1) {
        func_800824C8(p[-3]);
        func_800824DC(p[-2]);
        func_8007EE84(9, 0, 0, -1);
    }
    *p = 0;
}
