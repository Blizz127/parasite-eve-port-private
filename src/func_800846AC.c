extern int D_8009B764;
extern int D_8009B768;
extern int D_8009B778;
extern int D_8009B77C[];
extern char *D_8009B7BC;
extern char D_800A5B70[];
extern void func_80084EB0(char *a0);
extern void func_800847A0(char *a0);
extern int func_80082E00(char *a0);

void func_800846AC(int arg) {
    register int mode asm("$5");
    register int neg asm("$19");
    char *base;
    int *tbl;
    int v;
    int idx;
    char *slot;
    char *p;

    mode = arg;
    base = D_800A5B70;
    neg = -9;
    tbl = D_8009B77C;
    do {
        idx = D_8009B764;
        slot = (char *)((idx * 0xF0) + (int)base);
        if (mode != neg) {
            if (mode == 0) {
                asm("" : "=r"(mode) : "0"(mode));
                tbl[idx] = 0;
            } else {
                func_80084EB0(slot);
                func_800847A0(slot);
            }
        }
        p = D_8009B7BC;
        idx = D_8009B764;
        D_8009B768 = 0;
        *(short *)(p + 10) = 0;
        idx = idx + 1;
        D_8009B764 = idx;
        if (D_8009B778 >= idx) {
            v = func_80082E00((char *)((idx * 0xF0) + (int)base));
            mode = 0xFFFF;
            goto test;
        }
        v = 1;
        mode = 0xFFFF;
        asm volatile("");
    test:
        ;
    } while (v == 0);
}
