typedef struct {
    unsigned int lo;
    unsigned int hi;
} S64;

extern S64 func_80072DF4(S64 x, S64 y);

S64 func_800738D8(unsigned int a, unsigned int b)
{
    S64 acc;
    S64 t;
    unsigned int al;
    unsigned int bl;
    register unsigned int m asm("$2");
    register unsigned int p1 asm("$3");

    al = a & 0xFFFF;
    bl = b & 0xFFFF;
    p1 = al * bl;
    acc.lo = p1;
    acc.hi = 0;
    a = a >> 16;
    m = a * bl;
    t.hi = m >> 16;
    t.lo = m << 16;
    acc = func_80072DF4(acc, t);
    b = b >> 16;
    m = al * b;
    t.hi = m >> 16;
    t.lo = m << 16;
    acc = func_80072DF4(acc, t);
    m = acc.hi;
    p1 = a * b;
    m = m + p1;
    acc.hi = m;
    return acc;
}
