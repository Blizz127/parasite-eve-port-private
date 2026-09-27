typedef struct {
    unsigned int lo;
    unsigned int hi;
} S64;
typedef union {
    S64 w;
    double d;
} DU;

extern S64 func_80072EA4(int dir, S64 v, int n);
extern int func_800739C4(int a0, int a1);

int func_80073354(double d)
{
    DU u;
    int e;
    int sh;
    S64 m;
    register int mh asm("$3");
    register int rv asm("$2");

    u.d = d;
    if ((u.w.hi & 0x7FFFFFFF) == 0 && u.w.lo == 0) {
        return u.w.hi & 0x80000000;
    }
    e = ((int)u.w.hi >> 20) & 0x7FF;
    sh = e - 0x41D;
    if (sh > 0) {
        func_800739C4(0x22, 0x11);
        if ((int)u.w.hi < 0) {
            return 0x80000000;
        }
        return 0x7FFFFFFF;
    }
    m.hi = (u.w.hi & 0xFFFFF) | 0x100000;
    m.lo = u.w.lo;
    m = func_80072EA4(0, m, 10);
    mh = m.hi;
    if ((unsigned int)(e - 0x3FE) >= 0x20) {
        goto zero;
    }
    asm volatile("" : "=r"(mh) : "0"(mh));
    if (mh == 0) {
        goto zero;
    }
    mh = mh >> -sh;
    goto sign;
zero:
    return 0;
sign:
    rv = mh;
    if ((int)u.w.hi < 0) {
        rv = -rv;
    }
    return rv;
}
