typedef struct {
    unsigned int lo;
    unsigned int hi;
} S64;

S64 func_80072DF4(S64 a, S64 b)
{
    unsigned int w[4];

    w[1] = a.lo >> 16;
    w[0] = a.lo & 0xFFFF;
    w[3] = b.lo >> 16;
    w[2] = b.lo & 0xFFFF;
    w[0] = w[0] + w[2];
    if ((w[0] & 0x10000) != 0) {
        w[1] = w[1] + 1;
    }
    w[1] = w[1] + w[3];
    if ((w[1] & 0x10000) != 0) {
        a.hi = a.hi + 1;
    }
    a.lo = (w[1] << 16) | (w[0] & 0xFFFF);
    a.hi = a.hi + b.hi;
    return a;
}
