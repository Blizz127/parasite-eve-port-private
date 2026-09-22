extern int D_800A3604;
extern int func_8007FBF0(int);
extern int func_8007FB44(unsigned char, int);

int func_8007E8F4(void)
{
    register int *p asm("$3");
    register int i asm("$4");
    int *e;

    if (func_8007FBF0(0) != 1) {
        return 0;
    }
    p = &D_800A3604;
    asm volatile("" : "=r"(p) : "0"(p));
    i = p[0];
    p -= 49;
    asm volatile("" : "=r"(p) : "0"(p));
    e = (int *)(i * 24 + (int)p);
    if (e[0] != 0) {
        return func_8007FB44(*(unsigned char *)((char *)e + 4), e[3]) != 0;
    }
    return 0;
}
