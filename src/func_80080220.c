typedef struct Quad {
    unsigned char b0;
    unsigned char b1;
    unsigned char b2;
    unsigned char b3;
} Quad;

extern unsigned char D_8009B558[];
extern unsigned char D_8009B56C[];
extern int D_8009B554;
extern void (*volatile D_800A36A4)(unsigned char a0, int a1);

void func_80080220(unsigned char a0, int a1)
{
    register unsigned char *p asm("$4");
    register unsigned char *r asm("$3");
    register int two asm("$6");
    register unsigned char ac asm("$7");
    register int a1c asm("$8");
    int m0;
    unsigned char *q;
    register unsigned char *sp2 asm("$5");
    register unsigned char *dp2 asm("$4");
    int *w;
    unsigned char c;
    int m;
    int val;
    int k;

    ac = a0;
    m0 = ac;
    a1c = a1;
    two = 2;
    if (m0 == two) {
        p = D_8009B558;
        asm ("" : "=r" (p) : "0" (p));
        c = p[0];
        k = 0xE;
        asm ("" : "=r" (c) : "0" (c));
        m = c;
        if (m == k) {
            if (((p[0x29] ^ p[1]) & 0x80) != 0) {
                *(int *)(p + 0x20) = 0xF;
                *(int *)(p + 0x1C) = two;
                *(int *)(p + 0x3C) = 3;
            } else {
                *(int *)(p + 0x1C) = 1;
                *(int *)(p + 0x20) = 0xB;
            }
            r = &D_8009B558[0x29];
            asm ("" : "=r" (r) : "0" (r));
            *r = r[-0x28];
        } else if (m == 3) {
            val = 0x10;
            goto setval;
        } else if (m == 6 || m == 0x1B) {
            val = 0x11;
setval:
            *(int *)(p + 0x20) = val;
            *(int *)(p + 0x1C) = two;
            p[0x2F] = c;
            *(int *)(p + 0x48) = 0x4B0;
        } else {
            switch (p[0]) {
            case 2:
                sp2 = &D_8009B558[1];
                asm ("" : "=r" (sp2) : "0" (sp2));
                dp2 = &D_8009B558[0x2A];
                asm ("" : "=r" (dp2) : "0" (dp2));
                *(Quad *)dp2 = *(Quad *)sp2;
                break;
            case 0x15:
            case 0x16:
                r = &D_8009B558[0x2E];
                asm ("" : "=r" (r) : "0" (r));
                *r = r[-0x2E];
                break;
            case 3:
            case 6:
            case 0x1B:
                r = &D_8009B558[0x2F];
                asm ("" : "=r" (r) : "0" (r));
                *r = r[-0x2F];
                break;
            }
            w = (int *)&D_8009B558[0x1C];
            asm ("" : "=r" (w) : "0" (w));
            w[0] = 1;
            w[1] = 0xB;
        }
    } else {
        q = D_8009B56C;
        asm ("" : "=r" (q) : "0" (q));
        if ((q[0] & 0x10) != 0) {
            *(int *)(q + 8) = two;
            *(int *)(q + 0xC) = 0xC;
        } else {
            *(int *)(q + 8) = 1;
            *(int *)(q + 0xC) = 0xB;
        }
    }
    if (D_800A36A4 != 0) {
        if (D_8009B554 != 0) {
            D_800A36A4(ac, a1c);
        }
    }
}
