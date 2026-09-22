extern int D_800A3530;
extern int D_800A3520;
extern void func_8007FC88(int);
extern void func_80080998(int, unsigned char *);

int func_8007F608(int a0)
{
    int t;

    func_8007FC88(0);
    if (D_800A3530 == 1) {
        t = 4;
    } else {
        t = (D_800A3520 == 1);
    }
    if (t == 4) {
        register int arg asm("$4") = a0;
        register int *p asm("$16") = &D_800A3530;
        asm volatile("" : "=r"(p) : "0"(p));
        p[0] = 0;
        func_80080998(arg, (unsigned char *)p + 5);
        return *((unsigned char *)p + 4);
    }
    if (t == 1) {
        register int arg asm("$4") = a0;
        register int *p asm("$16") = &D_800A3520;
        asm volatile("" : "=r"(p) : "0"(p));
        p[0] = 0;
        func_80080998(arg, (unsigned char *)p + 5);
        return *((unsigned char *)p + 4);
    }
    return 0;
}
