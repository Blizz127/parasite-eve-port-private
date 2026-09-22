extern int D_8009B424;
extern int *D_8009B464;
extern int D_8009B460;
extern int D_8009B45C;

int func_80085A04(int a0, int *a1)
{
    register int r asm("$2");
    int sh;

    r = a0;
    if (r <= 0) {
        r = 0;
    } else {
        sh = D_8009B424;
        a1[0] = 0x40001010;
        D_8009B464 = a1;
        D_8009B460 = 0;
        D_8009B45C = r;
        a1[1] = (0x10000 << sh) - 0x1010;
    }
    return r;
}
