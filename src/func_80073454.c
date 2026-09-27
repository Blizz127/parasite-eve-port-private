/*
 * func_80073454 - vram 0x80073454, size 0x100. libmath int -> double (Math_Int32ToDouble).
 * era: cc1 2.7.2 -O2 -G0 + MASPSX_FILL_EPILOGUE_DELAY_SLOT=1 (patch 3). Draft from lane4 (was 2w on
 * 2.8.1); exact on 2.7.2. DU union return in $s0/$s1 and the split hi |= e << 20 are load-bearing.
 */
typedef struct {
    unsigned int lo;
    unsigned int hi;
} S64;
typedef union {
    S64 w;
    double d;
} DU;
extern S64 func_80073294(int dir, S64 v, int n);
double func_80073454(int x) {
    unsigned int sign = 0;
    int e = 0x41D;
    DU r;
    S64 m;
    if (x == 0) {
        r.w.lo = 0;
        r.w.hi = 0;
    } else {
        if (x < 0) {
            sign = 0x80000000;
            x = -x;
        }
        while (x <= 0xFFFFFF) {
            x <<= 4;
            e -= 4;
        }
        while (x <= 0x3FFFFFFF) {
            x <<= 1;
            e -= 1;
        }
        m.lo = 0;
        m.hi = x;
        m = func_80073294(1, m, 10);
        m.hi &= 0xFFEFFFFF;
        r.w.hi = m.hi | sign;
        r.w.hi |= e << 20;
        r.w.lo = m.lo;
    }
    return r.d;
}
