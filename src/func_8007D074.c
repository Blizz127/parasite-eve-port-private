/*
 * func_8007D074 - vram 0x8007D074, size 0xE8. SPU init wrapper (in the 2.8.1 band but cc1 2.7.2).
 * era: cc1 2.7.2 -O2 -G0 + MASPSX_FILL_EPILOGUE_DELAY_SLOT=1 (patch 3; 2 words without).
 * 2.7.2 keeps retail epilogue order after the trailing stores (2.8.1 hoists lw ra/s0 above them).
 * Empty register barriers (no memory clobber): keep 0xC000 live across the bnez (delay-slot li),
 * keep the D_8009B3A0 struct base in a register, and order li a0,0xD1 before it. Loop pins $3/$2.
 */
extern unsigned short D_8009B3E6;
extern int D_8009B390;
extern int D_8009B394;
extern int D_8009B398;
extern int D_8009B46C;
extern int D_8009B45C;
extern int D_8009B460;
extern int D_8009B464;
extern int D_8009B38C;
extern int D_8009B418;
extern int D_8009B388;
extern int D_8009B3B4;
extern int D_8009B3B0;
extern int D_8009B3E8;
typedef struct {
    int w0;
    short h4;
    short h6;
    int w8;
    int wC;
} S3A0;
extern S3A0 D_8009B3A0;
extern void func_80073C94(void);
extern void func_8007D1D4(int);
extern void func_8007D15C(void);
extern void func_8007DAE0(int, int, int);

void func_8007D074(int a0)
{
    register int i asm("$3");
    register unsigned short *p asm("$2");
    int c;
    S3A0 *q;

    func_80073C94();
    func_8007D1D4(a0);
    c = 0xC000;
    if (a0 == 0) {
        __asm__("" : "=r"(c) : "0"(c));
        i = 0x17;
        p = &D_8009B3E6;
        for (; i >= 0; i--) {
            *p-- = c;
        }
    }
    func_8007D15C();
    {
        register int a asm("$4") = 0xD1;
        q = &D_8009B3A0;
        __asm__("" : "=r"(q) : "0"(q), "r"(a));
    }
    D_8009B390 = 0;
    D_8009B394 = 0;
    q->w0 = 0;
    q->h4 = 0;
    q->h6 = 0;
    q->w8 = 0;
    q->wC = 0;
    D_8009B398 = D_8009B46C;
    func_8007DAE0(0xD1, D_8009B46C, 0);
    D_8009B45C = 0;
    D_8009B460 = 0;
    D_8009B464 = 0;
    D_8009B38C = 0;
    D_8009B418 = 0;
    D_8009B388 = 0;
    D_8009B3B4 = 0;
    D_8009B3B0 = 0;
    D_8009B3E8 = 0;
}
