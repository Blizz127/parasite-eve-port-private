extern int D_8009B75C;
extern int D_8009B770;
extern void (*D_8009B724)();
extern void (*D_8009B728)();
extern void (*D_8009B72C)();
extern void (*D_8009B730)();
extern void (*D_8009B734)();
extern void (*D_8009B738)();
extern void (*D_8009B748)();
extern unsigned char *D_8009B758;
extern unsigned char D_800A5B70[];
extern unsigned char D_800A5AE0[];
extern unsigned char D_800A5B28[];
extern void func_80084B44(void);
extern void func_800846AC();
extern void func_80084644();
extern void func_800847B0();
extern void func_8008486C();
extern void func_80084AE8();
extern void func_80084B20();
extern void func_800847A0();
extern void func_80071A24(unsigned char *, int);
extern void func_80082ADC(void);

void func_800844E4(unsigned char *a0_, unsigned char *a1_)
{
    register unsigned char *a0 asm("$17") = a0_;
    register unsigned char *a1 asm("$18") = a1_;
    register unsigned char *ta asm("$6");
    register unsigned char *tb asm("$7");
    register int i asm("$8");
    register int ff asm("$9");
    unsigned char *r;
    register unsigned char *p asm("$4");
    unsigned char *q;
    int j;

    D_8009B75C = 0;
    D_8009B770 = 0;
    func_80084B44();
    r = D_800A5B70;
    D_8009B724 = func_800846AC;
    D_8009B728 = func_80084644;
    D_8009B72C = func_800847B0;
    D_8009B730 = func_8008486C;
    D_8009B734 = func_80084AE8;
    D_8009B738 = func_80084B20;
    D_8009B758 = r;
    D_8009B748 = func_800847A0;
    __asm__ volatile ("" : "=r"(p) : "0"(r));
    func_80071A24(p, 0x1E0);
    i = 0;
    ff = 0xFF;
    p = r + 0x40;
    __asm__ volatile ("" : "=r"(i), "=r"(ff), "=r"(p) : "0"(i), "1"(ff), "2"(p));
    tb = D_800A5B28;
    ta = D_800A5AE0;
    *(unsigned char **)(r + 0x30) = a0;
    *(unsigned char **)(r + 0x120) = a1;
    do {
        *(int *)(p - 0x34) = 0;
        *(unsigned char **)(p - 0x30) = r;
        (*(unsigned char **)(p - 0x10))[0] = ff;
        (*(unsigned char **)(p - 0x10))[1] = 0;
        *(unsigned char **)(p - 4) = ta;
        *(unsigned char **)p = tb;
        q = r + 0x5D;
        for (j = 5; j >= 0; j--) {
            *q++ = ff;
        }
        tb += 0x23;
        ta += 0x23;
        i += 1;
        p += 0xF0;
        r += 0xF0;
    } while (i < 2);
    func_80082ADC();
    D_8009B75C = 1;
    __asm__ volatile ("" : "=r"(a0), "=r"(a1), "=r"(r) : "0"(a0), "1"(a1), "2"(r));
}
