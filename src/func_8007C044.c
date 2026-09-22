extern volatile unsigned char *D_8009B27C;
extern unsigned char *D_8009B288;
extern unsigned int *D_8009B28C;
extern unsigned int *D_8009B2B0;
extern unsigned int *D_8009B2B4;
extern unsigned int *D_8009B2B8;
extern unsigned int *D_8009B2BC;
extern unsigned int *D_8009B2C0;

int func_8007C044(int a0, unsigned int a1)
{
    volatile int t;

    *D_8009B27C = 0;
    *D_8009B288 = 0x80;
    *D_8009B2B0 = 0x21020843;
    *D_8009B28C = 0x1325;
    *D_8009B2B4 |= 0x8000;
    *D_8009B2B8 = a0;
    *D_8009B2BC = a1 | 0x10000;
    while ((*D_8009B27C & 0x40) == 0) {
        ;
    }
    *D_8009B2C0 = 0x11400100;
    t = *D_8009B2C0;
    return 0;
}
