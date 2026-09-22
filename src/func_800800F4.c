extern int D_8009B59C;
extern int D_8009B5A4[];
extern void func_8007B9EC(void);
extern void func_8007B558(int, int, int, int);

int func_800800F4(void)
{
    int *p;
    unsigned char k;
    int v;

    func_8007B9EC();
    p = &D_8009B59C;
    asm volatile("" : "=r"(p) : "0"(p));
    p[0] = p[0] + 1;
    k = *((unsigned char *)p - 0x44);
    if (D_8009B5A4[k] != 0) {
        v = 0x3C0;
    } else {
        v = 0x1E;
    }
    p[-1] = v;
    func_8007B558(*((unsigned char *)p - 0x44), p[-15], 0, 1);
    return 0;
}
