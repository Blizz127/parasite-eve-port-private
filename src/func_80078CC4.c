#define LOW(MLO) \
    do { \
        asm volatile("" : "=r"(w) : "0"(w)); \
        t1 = (int)((unsigned int)w & 0xFFFF); \
        asm volatile("" : "=r"(t1) : "0"(t1)); \
        t1 = (t1 << 16) >> 16; \
        ulo = (unsigned int)t1; \
        asm volatile("" : "=r"(ulo) : "0"(ulo)); \
        ulo = ulo * (MLO); \
        t1 = (int)ulo >> 12; \
    } while (0)

#define HIGH(MHI) \
    do { \
        t2 = w >> 16; \
        asm volatile("" : "=r"(w), "=r"(t2) : "0"(w), "1"(t2)); \
        uhi = (unsigned int)t2; \
        asm volatile("" : "=r"(uhi) : "0"(uhi)); \
        uhi = uhi * (MHI); \
        t2 = (int)uhi >> 12; \
        t2 <<= 16; \
    } while (0)

int *func_80078CC4(int *base, unsigned int *m)
{
    register int *p asm("$4");
    register unsigned int a asm("$11");
    register unsigned int b asm("$12");
    register unsigned int c asm("$13");
    register int w asm("$8");
    register int t1 asm("$9");
    register int t2 asm("$10");
    unsigned int ulo;
    unsigned int uhi;

    p = base;
    asm volatile("" : "=r"(p) : "0"(p));
    a = m[0];
    b = m[1];
    c = m[2];
    asm volatile("" : "=r"(a), "=r"(b), "=r"(c) : "0"(a), "1"(b), "2"(c));

    w = p[0];
    LOW(a);
    t1 &= 0xFFFF;
    HIGH(b);
    p[0] = t1 | t2;
    asm volatile("" : "=r"(p) : "0"(p));

    w = p[1];
    LOW(c);
    t1 &= 0xFFFF;
    HIGH(a);
    p[1] = t1 | t2;
    asm volatile("" : "=r"(p) : "0"(p));

    w = p[2];
    LOW(b);
    t1 &= 0xFFFF;
    HIGH(c);
    p[2] = t1 | t2;
    asm volatile("" : "=r"(p) : "0"(p));

    w = p[3];
    LOW(a);
    t1 &= 0xFFFF;
    HIGH(b);
    p[3] = t1 | t2;
    asm volatile("" : "=r"(p) : "0"(p));

    w = p[4];
    LOW(c);
    p[4] = t1;
    return p;
}
