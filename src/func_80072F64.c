typedef struct {
    unsigned int lo;
    unsigned int hi;
} S64;
typedef union {
    S64 w;
    double d;
} DU;
extern int func_80073244(S64 a, S64 b);
extern S64 func_80072EA4(int dir, S64 v, int n);
extern S64 func_80073294(int dir, S64 v, int n);
extern S64 func_80073554(S64 v);
extern S64 func_80072DF4(S64 a, S64 b);
extern int func_800739C4(int a0, int a1);

double func_80072F64(double a, double b)
{
    S64 q, bit, rnd, ma, mb;
    DU ua, ub, r;
    unsigned int sign;
    int e;
    int n;

    ua.d = a;
    ub.d = b;
    e = (((int)ua.w.hi >> 20) & 0x7FF) - (((int)ub.w.hi >> 20) & 0x7FF) + 0x3FE;
    sign = (ua.w.hi & 0x80000000) ^ (ub.w.hi & 0x80000000);
    if ((ub.w.hi & 0x7FFFFFFF) == 0 && ub.w.lo == 0) {
        if (sign != 0) {
            r.w.hi = 0xFFFFFFFF;
        } else {
            r.w.hi = 0x7FFFFFFF;
        }
        r.w.lo = 0xFFFFFFFF;
        return r.d;
    }
    if ((ua.w.hi & 0x7FFFFFFF) == 0 && ua.w.lo == 0) {
        r.w.hi = sign;
        r.w.lo = 0;
        return r.d;
    }
    ma.hi = (ua.w.hi & 0xFFFFF) | 0x100000;
    ma.lo = ua.w.lo;
    mb.hi = (ub.w.hi & 0xFFFFF) | 0x100000;
    mb.lo = ub.w.lo;
    if (func_80073244(ma, mb) < 0) {
        ma = func_80072EA4(0, ma, 1);
        e--;
    }
    bit.hi = 0x200000;
    bit.lo = 0;
    q.lo = 0;
    q.hi = 0;
    do {
        if (func_80073244(ma, mb) >= 0) {
            q.hi |= bit.hi;
            q.lo |= bit.lo;
            ma = func_80072DF4(ma, func_80073554(mb));
        }
        ma = func_80072EA4(0, ma, 1);
        bit = func_80073294(1, bit, 1);
    } while (bit.hi != 0 || bit.lo != 0);
    if (e >= 0) {
        rnd.hi = 0;
        rnd.lo = 1;
        q = func_80072DF4(q, rnd);
        e++;
        q = func_80073294(1, q, 1);
    } else {
        n = -e;
        rnd.hi = 0;
        rnd.lo = 1 << n;
        q = func_80072DF4(q, rnd);
        e = 0;
        q = func_80073294(1, q, n + 1);
    }
    q.hi &= 0xFFEFFFFF;
    if (e >= 0x7FF) {
        func_800739C4(0x22, 0xF);
        {
            unsigned int h;
            if (sign != 0) {
                h = 0xFFF00000;
            } else {
                h = 0x7FF00000;
            }
            r.w.hi = h;
        }
        r.w.lo = 0;
    } else {
        r.w.hi = sign | (e << 20) | q.hi;
        r.w.lo = q.lo;
    }
    return r.d;
}
