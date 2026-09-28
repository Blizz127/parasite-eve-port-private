extern void func_80073C5C(char *);
extern void func_80071A74(char *, ...);

extern volatile unsigned char *D_8009B27C;
extern volatile unsigned char *D_8009B280;
extern volatile unsigned char *D_8009B284;
extern volatile unsigned char *D_8009B288;
extern unsigned char *D_8009B27C_nv asm("D_8009B27C");
extern unsigned char *D_8009B288_nv asm("D_8009B288");
extern unsigned char D_8009AFD5;
extern int D_8009AFC0;
extern unsigned int D_8009AFC4;
extern unsigned int D_8009AFC8;
extern int D_8009AFCC;
extern char *D_8009AFDC[];
extern int D_8009B07C[];
extern int D_8009B17C[];
extern volatile unsigned char D_8009B294[];
extern unsigned char D_800A3460[];
extern unsigned char D_800A3468[];
extern unsigned char D_800A3470[];
extern char D_80011B44[];
extern char D_80011B50[];
extern char D_80011B6C[];
extern char D_80011B80[];

static inline void copy8(unsigned char *d, volatile unsigned char *s, int n)
{
    if (d) {
        while (n--) {
            *d++ = *s++;
        }
    }
}
#define COPY8(dst, src) copy8((dst), (src), 8)

#define DBG(args)                               \
    if (D_8009AFC0 > 0) {                       \
        func_80071A74 args;                     \
    }

int func_8007AAB4(void)
{
    volatile unsigned char r;
    volatile unsigned char result[8];
    int i;
    int j;
    int err;

    *D_8009B27C = 1;
    r = *D_8009B288 & 7;
    err = 0;
    if (r == 0) {
        return 0;
    }
    while (r != (*D_8009B288 & 7)) {
        r = *D_8009B288 & 7;
    }
    for (i = 0; i < 8; i++) {
        if (!(*D_8009B27C & 0x20)) {
            break;
        }
        result[i] = *D_8009B280;
    }
    for (j = i; j < 8; j++) {
        result[j] = 0;
    }
    *D_8009B27C = 1;
    *D_8009B288 = 7;
    *D_8009B284 = 7;
    if (r != 3 || D_8009B17C[D_8009AFD5] != 0) {
        if (!(D_8009AFC4 & 0x10) && (result[0] & 0x10)) {
            D_8009AFCC++;
        }
        D_8009AFC4 = result[0];
        D_8009AFC8 = result[1];
        err = D_8009AFC4 & 0x1D;
    }
    if (r == 5) {
        DBG((D_80011B44));
        DBG((D_80011B50, D_8009AFDC[D_8009AFD5], D_8009AFC4, D_8009AFC8));
    }
    switch (r) {
    case 3:
        if (err) {
            D_8009B294[0] = 5;
            COPY8(D_800A3460, result);
            return 2;
        }
        if (D_8009B07C[D_8009AFD5]) {
            D_8009B294[0] = 3;
            COPY8(D_800A3460, result);
            return 1;
        }
        D_8009B294[0] = 2;
        COPY8(D_800A3460, result);
        return 2;
    case 2:
        D_8009B294[0] = err ? 5 : 2;
        COPY8(D_800A3460, result);
        return 2;
    case 1:
        if (err && i == 1) {
            err = 0;
        }
        {
            volatile unsigned char *q = D_8009B294;
            q[1] = err ? 5 : 1;
        }
        COPY8(D_800A3468, result);
        *D_8009B27C = 0;
        *D_8009B288_nv = 0;
        return 4;
    case 4:
        {
            volatile unsigned char *q = D_8009B294;
            q[1] = q[2] = 4;
        }
        COPY8(D_800A3470, result);
        COPY8(D_800A3468, result);
        return 4;
    case 5:
        D_8009B294[0] = D_8009B294[1] = 5;
        COPY8(D_800A3460, result);
        COPY8(D_800A3468, result);
        return 6;
    }
    func_80073C5C(D_80011B6C);
    func_80071A74(D_80011B80, r);
    return 0;
}
