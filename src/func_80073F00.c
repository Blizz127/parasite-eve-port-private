struct S {
    unsigned short f0;
    unsigned short f2;
    void (*tbl[11])();
    unsigned short f30;
};

extern struct S D_800945E4;
extern unsigned short D_800945E6;
extern unsigned short D_80094614;
extern unsigned short *D_80095670;
extern unsigned short *D_80095674;
extern int D_8009567C;
extern char D_80011740;
extern char D_8001175C;
extern void func_80071A74();
extern void func_80074384(void);

void func_80073F00(void)
{
    register struct S *b asm("$17");
    register int i asm("$17");
    register unsigned int v asm("$2");
    register unsigned short *pa asm("$4");
    register unsigned short *pb asm("$2");
    register unsigned int t1 asm("$3");
    register unsigned int one1 asm("$2");
    register unsigned short *pa2 asm("$5");
    register unsigned short *pb2 asm("$6");
    register int *cp asm("$2");
    register int nx asm("$3");
    register char *a0v asm("$4");
    register unsigned int m asm("$16");
    register void (**q)() asm("$18");
    register unsigned int one asm("$19");
    register void (**tbl)() asm("$20");
    void (*h)();
    register int c asm("$4");

    b = &D_800945E4;
    if (b->f0 == 0) {
        func_80071A74(&D_80011740, *D_80095670);
        func_80074384();
    }
    pa = D_80095670;
    t1 = b->f30;
    one1 = 1;
    b->f2 = one1;
    pb = D_80095674;
    v = *pb & (t1 & *pa);
    m = v;
    if (v != 0) {
        one = 1;
        tbl = b->tbl;
        do {
            i = 0;
            if (m != 0) {
                q = tbl;
            loop:
                if (i >= 0xB) {
                    goto done;
                }
                if ((m & 1) != 0) {
                    *D_80095670 = ~(one << i);
                    h = *q;
                    if (h != 0) {
                        h();
                    }
                }
                q++;
                m = m >> 1;
                i++;
                if ((m & 0xFFFF) != 0) {
                    goto loop;
                }
            }
        done:
            pa = D_80095670;
            t1 = D_80094614;
            pb = D_80095674;
            v = *pb & (t1 & *pa);
            m = v;
        } while (v != 0);
    }
    pa2 = D_80095670;
    pb2 = D_80095674;
    if ((*pa2 & *pb2) != 0) {
        cp = &D_8009567C;
        nx = *cp;
        c = nx;
        nx = nx + 1;
        *cp = nx;
        if (c >= 0x801) {
            a0v = &D_8001175C;
            __asm__ __volatile__("" : "=r"(a0v) : "0"(a0v));
            func_80071A74(a0v, *pa2, *pb2);
            D_8009567C = 0;
            *D_80095670 = 0;
        }
    } else {
        D_8009567C = 0;
    }
    D_800945E6 = 0;
    func_80074384();
}
