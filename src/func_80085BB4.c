extern unsigned int *D_8009B464;
extern int D_8009B424;

int func_80085BB4(unsigned int a0)
{
    unsigned int *q;
    unsigned int *p;
    unsigned int w;
    unsigned int x;
    unsigned int m1;
    unsigned int m2;
    unsigned int m3;
    int r;

    x = a0 << D_8009B424;
    q = D_8009B464;
    if (q == 0) {
        r = 0;
        goto done;
    }
    m1 = 0x80000000;
    m2 = 0x40000000;
    m3 = 0x0FFFFFFF;
    p = q;
    for (;;) {
        w = *p;
        if ((w & m1) == 0) {
            if ((w & m2) != 0) {
                goto zero;
            }
            w &= m3;
            if (w >= x) {
                r = 1;
                goto done;
            }
            if (x < w + p[1]) {
                r = 1;
                goto done;
            }
        }
        p += 2;
    }
zero:
    r = 0;
done:
    return r;
}
