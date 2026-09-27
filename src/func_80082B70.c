extern int D_8009B78C;
extern int D_8009B774;
extern int D_8009B778;
extern int D_8009B75C;
extern int D_8009B764;
extern int D_8009B768;
extern int D_8009B76C;
extern int D_800A5AC0;
extern int D_800A5AC4;
extern unsigned char *D_8009B758;
extern unsigned char *D_8009B788;
extern void (*D_8009B724)(int);
extern int func_80082E00(unsigned char *);
extern void func_80083014(unsigned char *);

int func_80082B70(void)
{
    int *p;
    int v;

    D_8009B78C = 1;
    if (D_8009B774 != 0) {
        p = &D_800A5AC0;
        v = *p;
        if (v < 0x96) {
            *p = v + 1;
        }
    }
    if (D_8009B778 == 0) {
        p = &D_800A5AC4;
        v = *p;
        if (v < 0x96) {
            *p = v + 1;
        }
    }
    if (D_8009B75C != 0 && D_8009B774 <= D_8009B778) {
        D_8009B768 = 0;
        D_8009B764 = D_8009B774;
        if (func_80082E00(D_8009B758 + D_8009B774 * 0xF0) == 0) {
            D_8009B724(0xFFFF);
        }
        D_8009B76C = 0;
        while (D_8009B764 <= D_8009B778) {
            func_80083014(D_8009B758 + D_8009B764 * 0xF0);
        }
        *(short *)(D_8009B788 + 0xE) = 0x88;
    }
    return 0;
}
