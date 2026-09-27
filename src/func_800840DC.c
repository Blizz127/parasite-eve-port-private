extern int (*D_8009B72C)(unsigned char *, int);
extern int func_800832B4(unsigned char *a0, int a1);

int func_800840DC(unsigned char *a0)
{
    int flag;
    int v;
    int r;

    flag = 0;
    if ((**(unsigned char **)(a0 + 0x3C) >> 4) == 8) {
        v = a0[0x36];
        flag = (v == 0);
    }
    r = D_8009B72C(a0, flag);
    r = func_800832B4(a0, r & 0xFF);
    if (r == 0x5A || r == 0 || r < 0) {
        return r;
    }
    return -9;
}
