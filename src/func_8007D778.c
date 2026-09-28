typedef struct {
    unsigned char voices[0x1A6];
    volatile unsigned short trans_addr; /* 0x1A6 */
    unsigned short data;           /* 0x1A8 */
    volatile unsigned short spucnt; /* 0x1AA */
} SpuRegs;

extern SpuRegs *D_8009B3FC;
extern unsigned int *volatile D_8009B400;   /* DMA MADR */
extern unsigned int *D_8009B404;   /* DMA BCR */
extern unsigned int *D_8009B408;   /* DMA CHCR */
extern unsigned short D_8009B414;
extern int D_8009B424;
extern int D_8009B44C;
extern volatile unsigned int D_8009B450;
extern unsigned int D_8009B454;
extern void func_8007DC84(void);
extern void func_8007DC5C(void);

int func_8007D778(int mode, ...) {
    int *ap = &mode + 1;
    unsigned int i;
    unsigned short want;
    unsigned int size;
    unsigned int chcr;

    switch (mode) {
    case 2:
        {
            register unsigned int x asm("$4") = ap[0];
            unsigned int t = D_8009B424;
            t = x >> t;
            D_8009B414 = t;
        }
        D_8009B3FC->trans_addr = D_8009B414;
        break;
    case 1:
        D_8009B44C = 0;
        for (i = 0; D_8009B3FC->trans_addr != D_8009B414;) {
            if (++i > 0xF00) {
                return -2;
            }
        }
        D_8009B3FC->spucnt = (D_8009B3FC->spucnt & ~0x30) | 0x20;
        break;
    case 0:
        D_8009B44C = 1;
        for (i = 0; D_8009B3FC->trans_addr != D_8009B414;) {
            if (++i > 0xF00) {
                return -2;
            }
        }
        D_8009B3FC->spucnt |= 0x30;
        break;
    case 3:
        want = D_8009B44C == 1 ? 0x30 : 0x20;
        for (i = 0; (D_8009B3FC->spucnt & 0x30) != want;) {
            if (++i > 0xF00) {
                return -2;
            }
        }
        if (D_8009B44C == 1) {
            func_8007DC84();
        } else {
            func_8007DC5C();
        }
        ap++;
        chcr = 0x1000000;
        asm("" : "=r"(chcr) : "0"(chcr));
        {
            register int y asm("$4") = ap[-1];
            D_8009B450 = y;
            asm("" :: "r"(y));
        }
        {
            register unsigned int sz asm("$4") = ap[0];
            unsigned int *p = D_8009B400;
            unsigned int hi = sz >> 6;
            unsigned int lo = (sz & 0x3F) != 0;
            unsigned int t = hi + lo;
            register unsigned int a asm("$4") = D_8009B450;
            D_8009B454 = t;
            *p = a;
        }
        *D_8009B404 = (D_8009B454 << 16) | 0x10;
        chcr |= 0x201;
        if (D_8009B44C == 1) {
            chcr = 0x1000200;
        }
        *D_8009B408 = chcr;
        break;
    }
    return 0;
}
