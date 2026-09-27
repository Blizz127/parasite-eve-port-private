extern unsigned char D_8009B554[];
extern unsigned char D_8009B560[];
extern unsigned char D_8009B56C[];
extern void func_80080B44(int, unsigned char *);

void func_8007FA2C(void)
{
    register int i asm("$3");
    register unsigned char *p asm("$2");

    i = 3;
    p = D_8009B554;
    {
        unsigned char *q = p + 8;

        *(int *)p = 0;
        p[4] = 0;
        do {
            *q-- = 0;
        } while (--i >= 0);
    }
    p = D_8009B560;
    *(int *)p = 0;
    i = 7;
    p += 0xB;
    do {
        *p-- = 0;
    } while (--i >= 0);
    D_8009B56C[0] = 0;
    *(int *)(D_8009B56C + 4) = 0;
    *(int *)(D_8009B56C + 8) = 2;
    *(int *)(D_8009B56C + 0xC) = 0xE;
    *(int *)(D_8009B56C + 0x10) = 0x15;
    D_8009B56C[0x14] = 0;
    D_8009B56C[0x15] = 0;
    func_80080B44(0, D_8009B56C + 0x16);
    D_8009B56C[0x1A] = 0;
    D_8009B56C[0x1B] = 0;
    D_8009B56C[0x1C] = 0;
    D_8009B56C[0x1D] = 0;
    D_8009B56C[0x1E] = 0;
    D_8009B56C[0x1F] = 0;
    *(int *)(D_8009B56C + 0x20) = 0;
    *(int *)(D_8009B56C + 0x24) = 1;
    *(int *)(D_8009B56C + 0x28) = 0;
    *(int *)(D_8009B56C + 0x30) = 0;
    *(int *)(D_8009B56C + 0x2C) = 0;
    *(int *)(D_8009B56C + 0x34) = 0;
}
