extern unsigned char D_8009B56C;
extern void (*volatile D_800A36A8)(int, int);
extern void func_8008080C(int);

void func_80080778(int a0, int a1)
{
    unsigned char *p;
    int c = a0 & 0xFF;

    func_8008080C(c);
    p = &D_8009B56C;
    asm volatile("" : "=r"(p) : "0"(p));
    if ((p[0] & 0x10) != 0) {
        *(int *)(p + 8) = 2;
        *(int *)(p + 0xC) = 0xC;
    }
    if (D_800A36A8 != 0) {
        if (*(int *)(p - 0x18) != 0) {
            D_800A36A8(c, a1);
        }
    }
}
