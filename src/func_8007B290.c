extern int func_80073A44(int);
extern int func_80073C5C(char *);
extern int func_80071A74(char *, ...);
extern void func_8007B9EC(void);
extern int func_80073DE8(void);
extern int func_8007AAB4(void);
extern int D_800A3478;
extern int D_800A347C;
extern char *D_800A3480;
extern char D_80011BA8[];
extern char D_80011B18[];
extern char D_80011B28[];
extern char *D_8009AFDC[];
extern char *D_8009B05C[];
extern volatile unsigned char D_8009B294[];
extern unsigned char D_8009AFD5;
extern volatile unsigned char *D_8009B27C;
extern void (*D_8009AFB8)();
extern void (*D_8009AFB4)();
extern unsigned char D_800A3468[];
extern unsigned char D_800A3460[];
extern unsigned char D_800A3470[];

int func_8007B290(int mode_, unsigned char *result_)
{
    register int mode asm("$23") = mode_;
    register unsigned char *result asm("$19") = result_;
    int st;
    int old;
    int c;
    unsigned char *s;
    int i;
    int t;
    int n;
    int o;
    char **comstr;
    char **intstr;
    volatile unsigned char *sync;
    volatile unsigned char *ready;
    volatile unsigned char *intr;
    int uc;
    register unsigned char *d asm("$5");

    { int m1; __asm__("" : "=r"(m1) : "0"(-1), "r"(result));
    D_800A3478 = func_80073A44(m1) + 0x3C0; }
    comstr = D_8009AFDC;
    intstr = D_8009B05C;
    sync = D_8009B294;
    ready = sync + 1;
    intr = sync + 2;
    D_800A347C = 0;
    D_800A3480 = D_80011BA8;
    while (1) {
        if (D_800A3478 < func_80073A44(-1) || (n = D_800A347C, o = n, n = n + 1, D_800A347C = n, o > 0x3C0000)) {
            func_80073C5C(D_80011B18);
            {
                int a = sync[0];
                int b = sync[1];
                char **pn = &D_800A3480;
                char *y = intstr[b];
                func_80071A74(D_80011B28, *pn, comstr[D_8009AFD5], intstr[a], y);
            }
            func_8007B9EC();
            t = -1;
        } else {
            t = 0;
        }
        if (t != 0) {
            return -1;
        }
        if (func_80073DE8() != 0) {
            old = *D_8009B27C & 3;
            while ((st = func_8007AAB4()) != 0) {
                if (st & 4) {
                    if (D_8009AFB8 != 0) {
                        D_8009AFB8(*ready, D_800A3468);
                    }
                }
                if (st & 2) {
                    if (D_8009AFB4 != 0) {
                        D_8009AFB4(*sync, D_800A3460);
                    }
                }
            }
            *D_8009B27C = old;
        }
        uc = *intr;
        if (uc != 0) {
            sync[2] = 0;
            d = result;
            s = D_800A3470;
            if (result != 0) {
                for (i = 7; i != -1; i--) {
                    *d++ = *s++;
                }
            }
            return uc;
        }
        uc = intr[-1];
        if (uc != 0) {
            sync[1] = 0;
            d = result;
            s = D_800A3468;
            if (d != 0) {
                for (i = 7; i != -1; i--) {
                    *d++ = *s++;
                }
            }
            return uc;
        }
        if (mode != 0) {
            return 0;
        }
    }
}
