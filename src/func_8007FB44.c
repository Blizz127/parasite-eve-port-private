extern int D_8009B598;
extern int func_8007FCFC(unsigned char);

int func_8007FB44(int a0)
{
    register int *p asm("$6") = &D_8009B598;
    register int *q asm("$7");

    asm volatile("" : "=r"(p) : "0"(p));
    if (p[0] > 0) {
        return 0;
    }
    if (p[-17] == 0) {
        return 0;
    }
    q = p - 17;
    asm volatile("" : "=r"(q) : "0"(q));
    if (p[-9] != 1) {
        return 0;
    }
    q[7] = 0x1F;
    p[-9] = 2;
    q[9] = 0xB;
    return func_8007FCFC(a0);
}
