typedef void (*CdCb)(unsigned char, unsigned char *);

extern int D_8009AFC0;
extern char *D_8009AFDC[];
extern char *D_8009B05C[];
extern int D_8009B0FC[2][64];
extern unsigned char D_8009AFD0[4];
extern unsigned char D_8009AFD4;
extern unsigned char D_8009AFD5;
extern volatile unsigned char D_8009B294[2];
extern volatile unsigned char *D_8009B27C;
extern volatile unsigned char *D_8009B280;
extern volatile unsigned char *D_8009B284;
extern int D_800A3478;
extern int D_800A347C;
extern char *D_800A3480;
extern unsigned char D_800A3460[];
extern unsigned char D_800A3468[];
extern CdCb D_8009AFB4;
extern CdCb D_8009AFB8;
extern char D_80011B18[];
extern char D_80011B28[];
extern char D_80011BB4[];
extern char D_80011BBC[];
extern char D_80011BCC[];
extern int func_80071A74(char *, ...);
extern void func_80073C5C(char *);
extern int func_8007B010(int, int);
extern int func_80073A44(int);
extern void func_8007B9EC(void);
extern int func_80073DE8(void);
extern int func_8007AAB4(void);

static inline void copy8(unsigned char *d, unsigned char *s, int n)
{
    if (d) {
        while (n--) {
            *d++ = *s++;
        }
    }
}

static __inline__ int timeout(char **names, char **inames, volatile unsigned char *s) {
    int t;

    if (func_80073A44(-1) > D_800A3478 || D_800A347C++ > 0x3C0000) {
        func_80073C5C(D_80011B18);
        {
            int a0 = s[0];
            char *i1 = inames[s[1]];
            char **pn = &D_800A3480;
            func_80071A74(D_80011B28, *pn, names[D_8009AFD5], inames[a0], i1);
        }
        func_8007B9EC();
        return -1;
    }
    return 0;
}

int func_8007B558(unsigned char com, unsigned char *param, unsigned char *result, int async) {
    int i;
    int *st;
    int mask;
    int intr;
    unsigned char *src;

    if (D_8009AFC0 >= 2) {
        func_80071A74(D_80011BB4, D_8009AFDC[com]);
    }
    if (D_8009B0FC[1][com] != 0 && param == 0) {
        if (D_8009AFC0 > 0) {
            func_80071A74(D_80011BBC, D_8009AFDC[com]);
        }
        return -2;
    }
    { int z = 0; __asm__("" : "=r"(z) : "0"(z)); func_8007B010(z, 0); }
    if (com == 2) {
        for (i = 0; i < 4; i++) {
            D_8009AFD0[i] = param[i];
        }
    }
    if (com == 14) {
        D_8009AFD4 = param[0];
    }
    D_8009B294[0] = 0;
    st = D_8009B0FC[0];
    if (st[com] != 0) {
        D_8009B294[1] = 0;
    }
    *D_8009B27C = 0;
    {
        int *np = st + 64;
        for (i = 0; i < np[com]; i++) {
            *D_8009B284 = param[i];
            asm("" : : "r"(param));
        }
    }
    D_8009AFD5 = com;
    *D_8009B280 = com;
    if (async != 0) {
        return 0;
    }
    D_800A3478 = func_80073A44(-1) + 960;
    D_800A347C = 0;
    D_800A3480 = D_80011BCC;
    {
        if (D_8009B294[0] == 0) {
            char **names = D_8009AFDC;
            char **inames = D_8009B05C;
            volatile unsigned char *s = D_8009B294;
            volatile unsigned char *s1 = s + 1;

            do {
                if (timeout(names, inames, s) != 0) {
                    return -1;
                }
                if (func_80073DE8() != 0) {
                    mask = *D_8009B27C & 3;
                    while ((intr = func_8007AAB4()) != 0) {
                        if ((intr & 4) && D_8009AFB8 != 0) {
                            D_8009AFB8(*s1, D_800A3468);
                        }
                        if ((intr & 2) && D_8009AFB4 != 0) {
                            D_8009AFB4(s[0], D_800A3460);
                        }
                    }
                    *D_8009B27C = mask;
                }
            } while (s[0] == 0);
        }
    }
    copy8(result, D_800A3460, 8);
    return D_8009B294[0] == 5 ? -1 : 0;
}
