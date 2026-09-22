extern unsigned char D_8009B558;
extern unsigned char D_8009B56C;
extern void (*volatile D_800A36A4)(int);

void func_8008068C(int a0)
{
    register int c asm("$7") = a0 & 0xFF;

    if (c == 2) {
        register unsigned char *p asm("$6") = &D_8009B558;
        asm volatile("" : "=r"(p) : "0"(p));
        if (p[0] == 0xE) {
            unsigned char u = p[0x29];
            unsigned char t = p[1];
            if (((u ^ t) & 0x80) != 0) {
                *(int *)(p + 0x20) = 0xF;
                *(int *)(p + 0x1C) = c;
                *(int *)(p + 0x3C) = 3;
            }
            p[0x29] = t;
        }
    }
    if ((a0 & 0xFF) == 5) {
        register unsigned char *q asm("$4") = &D_8009B56C;
        asm volatile("" : "=r"(q) : "0"(q));
        if ((q[0] & 0x10) != 0) {
            register void (*f)(int) asm("$3") = D_800A36A4;
            *(int *)(q + 8) = 2;
            *(int *)(q + 0xC) = 0xC;
            if (f != 0) {
                if (*(int *)(q - 0x18) != 0) {
                    D_800A36A4(5);
                }
            }
        } else {
            *(int *)(q + 8) = 1;
            *(int *)(q + 0xC) = 0xB;
        }
    }
}
