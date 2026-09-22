extern unsigned char D_8009B558;
extern int D_8009B624[];
extern unsigned char D_8009B588;
extern void func_80080998(unsigned char *, unsigned char *);

void func_8008080C(int a0, unsigned char *a1)
{
    register unsigned char *q asm("$6") = a1;
    unsigned char *p;
    int off;
    register unsigned char b asm("$5");
    unsigned int v;

    if ((a0 & 0xFF) != 5) {
        off = D_8009B624[D_8009B558] - 1;
        if (off < 0) {
            return;
        }
    } else {
        off = 0;
    }
    b = q[off];
    v = b & 0xFF;
    p = &D_8009B588;
    asm volatile("" : "=r"(p) : "0"(p));
    p[0] = v >> 7;
    p[1] = (v >> 6) & 1;
    p[2] = (v >> 5) & 1;
    p[3] = (v >> 1) & 1;
    asm volatile("" : "=r"(b) : "0"(b));
    p[-0x1C] = b;
    p -= 0x24;
    asm volatile("" : "=r"(p) : "0"(p));
    func_80080998(p, q);
}
