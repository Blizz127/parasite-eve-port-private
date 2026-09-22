extern int D_800A3608;

void *func_8007E6B0(void)
{
    register int *p asm("$3") = &D_800A3608;
    register int n asm("$4");
    int i;

    asm volatile("" : "=r"(p) : "0"(p));
    n = *p;
    if (n >= 8) {
        return 0;
    }
    i = p[-2] + n;
    if (i >= 8) {
        i -= 8;
    }
    {
        register int m asm("$2") = i * 24;
        register int q asm("$3") = (int)p - 200;
        return (void *)(m + q);
    }
}
