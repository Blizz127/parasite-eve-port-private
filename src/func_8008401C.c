extern int D_8009B7A4;
extern void (*D_8009B73C)(int a0);
extern int D_8009B770;
extern int D_8009B79C;
extern int func_800832B4(unsigned char *a0, int a1);

int func_8008401C(unsigned char *a0)
{
    int arg;
    int r;
    int t;

    if (D_8009B7A4 != 0) {
        D_8009B73C(*(int *)(a0 + 0xC) + 0x1E0);
        D_8009B73C(*(int *)(a0 + 0xC) + 0x2D0);
    }
    arg = 0;
    if (a0[0x36] == 0) {
        arg = D_8009B770;
    }
    r = func_800832B4(a0, arg);
    if (r < 0) {
        return r;
    }
    if ((r & 0xF0) == 0) {
        return -9;
    }
    t = (r & 0xF) << 1;
    D_8009B79C = t;
    if (t != 0) {
        return 0;
    }
    D_8009B79C = 0x20;
    return 0;
}
