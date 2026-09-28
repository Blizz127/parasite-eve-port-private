typedef struct {
    unsigned int lo;
    unsigned int hi;
} S64;
typedef union {
    S64 w;
    double d;
} DU;
extern S64 func_80073294(int dir, S64 v, int n);
extern S64 func_800738D8(unsigned int a, unsigned int b);
extern S64 func_80072DF4(S64 a, S64 b);
extern int func_800739C4(int a0, int a1);

double func_800735C4(double a, double b)
{
    S64 acc, rnd, ma, mb, t, p;
    volatile DU r;
    DU ua, ub;
    unsigned int sign;
    volatile int e;
    int x;
    unsigned int ah, bh, al, bl;
    int n;
    unsigned int hb;

    ua.d = a;
    ub.d = b;
    sign = (ua.w.hi & 0x80000000) ^ (ub.w.hi & 0x80000000);
    if (((ua.w.hi & 0x7FFFFFFF) == 0 && ua.w.lo == 0) ||
        ((ub.w.hi & 0x7FFFFFFF) == 0 && ub.w.lo == 0)) {
        r.w.hi = sign;
        r.w.lo = 0;
    } else {
        {
            register int ea asm("$3") = ((int)ua.w.hi >> 20) & 0x7FF;
            register int v asm("$3") = ea + (((int)ub.w.hi >> 20) & 0x7FF);
            x = v - 0x3FE;
            e = v;
        }
        {
            register unsigned int mk asm("$3") = 0xFFFFF;
            ma.hi = (ua.w.hi & mk) | 0x100000;
            ma.lo = ua.w.lo;
            mb.hi = (ub.w.hi & mk) | 0x100000;
            mb.lo = ub.w.lo;
        }
        t = func_80073294(1, ma, 21);
        ah = t.lo;
        t = func_80073294(1, mb, 21);
        bh = t.lo;
        al = ma.lo & 0x1FFFFF;
        bl = mb.lo & 0x1FFFFF;
        acc = func_800738D8(ah, bh);
        p = func_800738D8(al, bh);
        t = func_80073294(1, p, 21);
        acc = func_80072DF4(acc, t);
        p = func_800738D8(bl, ah);
        t = func_80073294(1, p, 21);
        acc = func_80072DF4(acc, t);
        if ((int)acc.hi < 0) {
            rnd.hi = 0;
            rnd.lo = 0x400;
            acc = func_80072DF4(acc, rnd);
            acc = func_80073294(1, acc, 11);
        } else {
            hb = 0x80000000;
            __asm__("" : "=r"(hb) : "0"(hb));
            rnd.hi = 0;
            rnd.lo = 0x200;
            acc = func_80072DF4(acc, rnd);
            if (acc.hi & hb) {
                acc = func_80073294(1, acc, 11);
            } else {
                {
                    register int ev asm("$11") = e;
                    x = ev - 0x3FF;
                }
                acc = func_80073294(1, acc, 10);
            }
        }
        acc.hi &= 0xFFEFFFFF;
        if (x >= 0x7FF) {
            func_800739C4(0x22, 0xD);
            if (sign != 0) {
                r.w.hi = 0xFFF00000;
            } else {
                r.w.hi = 0x7FF00000;
            }
            r.w.lo = 0;
        } else {
            r.w.hi = sign | (x << 20) | acc.hi;
            {
                register unsigned int lv asm("$11") = ((volatile S64 *)&acc)->lo;
                r.w.lo = lv;
            }
        }
    }
    return r.d;
}
