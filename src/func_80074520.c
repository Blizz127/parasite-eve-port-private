extern volatile int *D_800956BC;
extern void (*D_800956C0[7])();
extern int *D_800956E0;
extern char D_8001177C;
extern char D_80011798;
extern void func_80071A74();

void func_80074520(void)
{
    register unsigned int m asm("$17");
    register int i asm("$16");
    register void (**p)() asm("$18");
    register unsigned int one asm("$20");
    register unsigned int mask asm("$19");
    register void (**base)() asm("$21");
    register volatile int *f asm("$4");
    register volatile int *g asm("$5");
    register int a1v asm("$5");
    register unsigned int t asm("$2");
    register char *a0v asm("$4");
    void (*h)();

    t = (unsigned int)*D_800956BC >> 24;
    m = t & 0x7F;
    if (m != 0) {
        one = 1;
        mask = 0xFFFFFF;
        base = D_800956C0;
        do {
            i = 0;
            if (m != 0) {
                p = base;
            loop:
                if (i >= 7) {
                    goto done;
                }
                if ((m & 1) != 0) {
                    f = D_800956BC;
                    *f = *f & ((one << (i + 0x18)) | mask);
                    h = *p;
                    if (h != 0) {
                        h();
                    }
                }
                p++;
                m = m >> 1;
                i++;
                if (m != 0) {
                    goto loop;
                }
            done: ;
            }
            t = (unsigned int)*D_800956BC >> 24;
            m = t & 0x7F;
        } while (m != 0);
    }
    g = D_800956BC;
    if ((*g & 0xFF000000) == 0x80000000 || (*g & 0x8000) != 0) {
        a0v = &D_8001177C;
        __asm__ __volatile__("" : "=r"(a0v) : "0"(a0v));
        func_80071A74(a0v, *g);
        for (i = 0; i < 7; i++) {
            a0v = &D_80011798;
            __asm__ __volatile__("" : "=r"(a0v) : "0"(a0v));
            a1v = i;
            __asm__ __volatile__("" : "=r"(a1v) : "0"(a1v));
            func_80071A74(a0v, a1v, D_800956E0[i * 4]);
        }
    }
}
