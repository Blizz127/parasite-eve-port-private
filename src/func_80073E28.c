extern unsigned char D_800945E4[];
extern unsigned short *D_80095670;
extern unsigned short *D_80095674;
extern int *D_80095678;
extern int *volatile D_8009566C;
extern unsigned char D_80094620[];
extern void func_80074330(void *a0, int a1);
extern int func_80074354(void *a0);
extern void func_80073F00(void);
extern void func_800743A4(void *a0);
extern int func_800743B4(void);
extern int func_800744D4(void);
extern int func_8007436C(void);
extern void func_80072724(void);

void *func_80073E28(void)
{
    unsigned char *p = D_800945E4;
    unsigned char *q;
    volatile unsigned short *a;
    unsigned short *b;
    register int *t asm("$4");
    int r;

    __asm__ __volatile__("" : "=r"(p) : "0"(p));
    if (*(unsigned short *)p != 0) {
        return 0;
    }
    b = D_80095670;
    a = (volatile unsigned short *)D_80095674;
    *a = 0;
    *b = *a;
    *D_80095678 = 0x33333333;
    func_80074330(p, 0x41A);
    if (func_80074354(p + 0x38) != 0) {
        func_80073F00();
    }
    q = D_80094620;
    __asm__ __volatile__("" : "=r"(q) : "0"(q));
    *(int *)q = (int)(q + 0xFDC);
    func_800743A4(q - 4);
    *(short *)(q - 0x3C) = 1;
    D_8009566C[5] = func_800743B4();
    r = func_800744D4();
    t = D_8009566C;
    t[1] = r;
    func_8007436C();
    __asm__ __volatile__("" : "=r"(q) : "0"(q));
    q = q - 0x3C;
    func_80072724();
    return q;
}
