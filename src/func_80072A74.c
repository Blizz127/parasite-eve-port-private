typedef struct {
    unsigned int lo;
    unsigned int hi;
} S64;
typedef union {
    S64 w;
    double d;
} DU;
extern S64 func_80072EA4(int dir, S64 v, int n);
extern S64 func_80073554(S64 v);
extern S64 func_80072DF4(S64 a, S64 b);
extern int func_800739C4(int a0, int a1);

double func_80072A74(double a, double b)
{
    S64 ma, mb, rnd;
    DU z;
    DU ua, ub;
    unsigned int sign;
    int ea, eb;
    register unsigned int hb asm("$21");
    unsigned int mx;
    unsigned int n;

    z.w.lo = 0;
    __asm__("" :: "m"(z.w.lo));
    z.w.hi = 0;
    sign = 0;
    ua.d = a;
    ub.d = b;
    if ((ua.w.hi & 0x7FFFFFFF) == 0 && ua.w.lo == 0) {
        return b;
    }
    if ((ub.w.hi & 0x7FFFFFFF) == 0 && ub.w.lo == 0) {
        return ua.d;
    }
    ea = ((int)ua.w.hi >> 20) & 0x7FF;
    eb = ((int)ub.w.hi >> 20) & 0x7FF;
    if (eb + 54 < ea) {
        return ua.d;
    }
    if (ea + 54 < eb) {
        return ub.d;
    }
    ma.hi = (ua.w.hi & 0xFFFFF) | 0x100000;
    ma.lo = ua.w.lo;
    mb.hi = (ub.w.hi & 0xFFFFF) | 0x100000;
    mb.lo = ub.w.lo;
    hb = 0x80000000;
    if ((int)ua.w.hi < 0) {
        ma = func_80073554(ma);
    }
    if (ub.w.hi & hb) {
        __asm__("" :: "r"(hb));
        mb = func_80073554(mb);
    }
    ma = func_80072EA4(0, ma, 9);
    mb = func_80072EA4(0, mb, 9);
    if (eb < ea) {
        mb = func_80072EA4(1, mb, ea - eb);
    } else {
        ma = func_80072EA4(1, ma, eb - ea);
        ea = eb;
    }
    ma = func_80072DF4(ma, mb);
    if ((int)ma.hi < 0) {
        ma = func_80073554(ma);
        sign = 0x80000000;
    } else if (ma.hi == 0 && ma.lo == 0) {
        __asm__("" : "=m"(z));
        return z.d;
    }
    {
        register unsigned int me asm("$16") = 0xE0000000;
    if ((ma.hi & 0xE0000000) == 0) {
        do {
            ma = func_80072EA4(0, ma, 1);
            ea--;
        } while ((ma.hi & me) == 0);
    }
    }
    {
    register unsigned int mf asm("$16") = 0x40000000;
    if (ma.hi & mf) {
        ma = func_80072EA4(1, ma, 1);
        ea++;
    }
    n = 0xFF;
    rnd.hi = 0;
    if (ma.lo & 0x200) {
        n = 0x100;
    }
    rnd.lo = n;
    ma = func_80072DF4(ma, rnd);
    if (ma.hi & mf) {
        ma = func_80072EA4(1, ma, 1);
        ea++;
    }
    }
    ma = func_80072EA4(1, ma, 9);
    ma.hi &= 0xFFEFFFFF;
    if (ea >= 0x7FF) {
        func_800739C4(0x22, 0xB);
        {
            unsigned int h;
            if (sign != 0) {
                h = 0xFFF00000;
            } else {
                h = 0x7FF00000;
            }
            ua.w.hi = h;
        }
        ua.w.lo = 0;
    } else {
        ua.w.hi = sign | (ea << 20) | ma.hi;
        ua.w.lo = ma.lo;
    }
    return ua.d;
}
