extern int D_8009B44C;
extern volatile unsigned short *D_8009B3FC;
extern volatile int *D_8009B400;
extern volatile int *D_8009B404;
extern volatile int *D_8009B408;
extern void func_8007DCAC(void);
extern void func_8007DC84(void);

void func_8007D6D0(int a0, unsigned short a1, int a2)
{
    D_8009B3FC[0xD3] = a1;
    func_8007DCAC();
    D_8009B3FC[0xD5] |= 0x30;
    func_8007DCAC();
    func_8007DC84();
    *D_8009B400 = a0;
    *D_8009B404 = (a2 << 16) | 0x10;
    D_8009B44C = 1;
    *D_8009B408 = 0x1000200;
}
