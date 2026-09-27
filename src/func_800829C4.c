extern int D_8009B778;
extern int D_8009B774;
extern int D_8009B75C;
extern int D_800A5AC0;
extern int D_800A5AC4;
extern unsigned char *D_8009B758;
extern void (*D_8009B728)(unsigned char *);

int func_800829C4(int a0)
{
    int *p;
    int *q;
    int cur = (D_8009B778 << 1) | (D_8009B774 == 0);

    if (cur == a0) {
        return cur;
    }
    D_8009B75C = 0;
    if (a0 & 1) {
        D_8009B774 = 0;
        p = &D_800A5AC0;
        if (*p >= 0x96) {
            D_8009B728(D_8009B758);
        }
        *p = 0;
    } else {
        D_8009B774 = 1;
    }
    if (a0 & 2) {
        D_8009B778 = 1;
        q = &D_800A5AC4;
        if (*q >= 0x96) {
            D_8009B728(D_8009B758 + 0xF0);
        }
        *q = 0;
    } else {
        D_8009B778 = 0;
    }
    D_8009B75C = 1;
    return cur;
}
