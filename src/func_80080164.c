extern int D_8009B598;
extern void func_8008080C(int);
extern void func_80080220(int, int);
extern void func_80080404(int, int);
extern void func_8008068C(int, int);

void func_80080164(int a0, int a1)
{
    int b = a1;
    int c = a0;
    int *p;
    int m;
    unsigned char f;

    func_8008080C(c & 0xFF);
    p = &D_8009B598;
    asm volatile("" : "=r"(p) : "0"(p));
    f = *((unsigned char *)p - 0x2C);
    p[1] = 0;
    p[0] = 0;
    if ((f & 0x10) != 0) {
        c = 5;
    }
    m = p[-10];
    switch (m) {
    case 0x1F:
        func_80080220(c & 0xFF, b);
        break;
    case 0x20:
        func_80080404(c & 0xFF, b);
        break;
    default:
        func_8008068C(c & 0xFF, b);
        break;
    }
    p = &D_8009B598;
    asm volatile("" : "=r"(p) : "0"(p));
    if (p[0] == 0) {
        p[-10] = 0x21;
    }
}
