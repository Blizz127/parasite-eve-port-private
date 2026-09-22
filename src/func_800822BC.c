extern int D_8009B700[4];
extern void func_800824C8(int a0);
extern void func_800824DC(int a0);

void func_800822BC(void)
{
    int *p;

    p = &D_8009B700[3];
    asm volatile("" : "=r" (p) : "0" (p));
    if (*p == 1) {
        func_800824C8(p[-3]);
        func_800824DC(p[-2]);
    }
    *p = 0;
}
