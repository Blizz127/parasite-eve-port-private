extern unsigned char *D_8009B27C;
extern void (*D_8009AFB4)(unsigned char a0, unsigned char *a1);
extern void (*D_8009AFB8)(unsigned char a0, unsigned char *a1);
extern unsigned char D_8009B295;
extern unsigned char D_800A3460;
extern unsigned char D_800A3468;
extern int func_8007AAB4(void);

void func_8007C13C(void)
{
    unsigned char *p;
    unsigned char *q;
    int save;
    int m;
    void (*f)(unsigned char a0, unsigned char *a1);

    p = &D_8009B295;
    q = p - 1;
    save = *D_8009B27C & 3;
    for (;;) {
        m = func_8007AAB4();
        if (m == 0) {
            break;
        }
        if (m & 4) {
            f = D_8009AFB8;
            if (f != 0) {
                f(*p, &D_800A3468);
            }
        }
        if (m & 2) {
            f = D_8009AFB4;
            if (f != 0) {
                f(*q, &D_800A3460);
            }
        }
    }
    *D_8009B27C = save;
}
