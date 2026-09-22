extern int D_8009B598;
extern int func_8007FCFC(unsigned char);

int func_8007FCBC(int a0)
{
    int *p = &D_8009B598;

    asm volatile("" : "=r"(p) : "0"(p));
    if (p[0] > 0) {
        return 0;
    }
    p[-10] = 0x20;
    return func_8007FCFC(a0);
}
