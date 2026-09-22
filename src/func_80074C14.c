extern unsigned char D_8009574D;
extern void (*D_80095748)();
extern unsigned int *D_80095744;
extern char D_80011840;
extern void func_80073CF4(int a0, int a1);

int func_80074C14(int a0)
{
    register unsigned char *p asm("$17");
    int old;
    int (*h)();

    p = &D_8009574D;
    old = p[0];
    if (p[1] >= 2) {
        D_80095748(&D_80011840, a0);
    }
    if (a0 != p[0]) {
        h = (int (*)())D_80095744[13];
        h(1);
        p[0] = a0;
        func_80073CF4(2, 0);
    }
    return old;
}
