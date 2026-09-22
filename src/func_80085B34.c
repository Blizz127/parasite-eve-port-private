extern unsigned int *D_8009B464;

int func_80085B34(unsigned int a0)
{
    unsigned int *q;
    unsigned int *p;
    unsigned int w;
    unsigned int m1;
    unsigned int m2;
    unsigned int m3;
    int r;

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
            if (w >= a0) {
                r = 1;
                goto done;
            }
            if (a0 < w + p[1]) {
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
