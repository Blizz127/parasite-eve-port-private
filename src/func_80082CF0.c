extern int D_8009B75C;
extern int *D_8009B784;
extern unsigned char *volatile D_8009B758;
extern void (*volatile D_8009B728)(unsigned char *a0);
extern unsigned char D_800A5AB0;
extern unsigned char D_800A5AC0;
extern void func_80072714(void);
extern void func_8007E1F4(int a0, unsigned char *a1);
extern void func_8007E1E4(int a0, unsigned char *a1);
extern void func_80073C84(int a0, int a1);
extern void func_80072724(void);

void func_80082CF0(void)
{
    unsigned char *base;
    register int *p asm("$3");
    unsigned char *q;

    D_8009B75C = 0;
    func_80072714();
    base = &D_800A5AB0;
    func_8007E1F4(2, base);
    func_8007E1E4(2, base);
    p = D_8009B784;
    p[0] = -2;
    __asm__ ("" : "=r"(p) : "0"(p));
    p[1] = p[1] | 1;
    func_80073C84(3, 0);
    func_80072724();
    D_8009B728(D_8009B758);
    D_8009B728(D_8009B758 + 0xF0);
    q = &D_800A5AC0;
    __asm__ ("" : "=r"(q) : "0"(q));
    *(int *)(q + 4) = 0;
    *(int *)(q + 0) = 0;
    D_8009B75C = 1;
}
