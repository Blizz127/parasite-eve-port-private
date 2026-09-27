extern unsigned char *D_8009B4B0;

int func_8007E0F8(void)
{
    volatile int c[3];

    *(short *)(D_8009B4B0 + 0xA) = 0;
    c[0] = 10;
    while (--c[0] != -1) {
    }
    return 0;
}
