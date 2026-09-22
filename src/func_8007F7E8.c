extern int D_800A3608;
extern int D_800A3604;
extern int func_8007FBF0(int);
extern void func_8007FB44(unsigned char, int);

void func_8007F7E8(void)
{
    register int r asm("$16");
    register int *p asm("$17");
    register int i asm("$3");
    register int m asm("$2");
    int *e;

    r = func_8007FBF0(0);
    if (r != 1) {
        return;
    }
    p = &D_800A3608;
    asm volatile("" : "=r"(p) : "0"(p));
    if (p[0] <= 0) {
        return;
    }
    if (func_8007FBF0(0) != r) {
        return;
    }
    i = D_800A3604;
    m = i * 24;
    {
        register int q asm("$3") = (int)p - 0xC8;
        e = (int *)(m + q);
    }
    if (e[0] == 0) {
        return;
    }
    func_8007FB44(*(unsigned char *)((char *)e + 4), e[3]);
}
