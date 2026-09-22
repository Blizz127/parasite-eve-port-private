typedef struct {
    unsigned int lo;
    unsigned int hi;
} S64;

extern S64 func_80072DF4(S64 a, S64 b);

S64 func_80073554(S64 x)
{
    S64 one;

    one.hi = 0;
    one.lo = 1;
    x.hi = ~x.hi;
    x.lo = ~x.lo;
    x = func_80072DF4(x, one);
    return x;
}
