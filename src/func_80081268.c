extern int D_8009B6CC;
extern void func_8007F69C(void);
extern void func_800822BC(void);
extern void func_800824F0(int);
extern void func_8007FCBC(int, int);
extern void func_8007EE84(int, int, int, int);

void func_80081268(void)
{
    int *p = &D_8009B6CC;

    asm volatile("" : "=r"(p) : "0"(p));
    if (p[0] == 1) {
        func_8007F69C();
        func_800822BC();
        if ((p[-5] & 1) != 0) {
            func_800824F0(p[-3]);
        }
        func_8007FCBC(1, 0);
        func_8007EE84(9, 0, 0, -1);
    }
    D_8009B6CC = 0;
}
