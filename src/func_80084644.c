void func_80084644(unsigned char *a0)
{
    unsigned char *p;
    unsigned char fill;
    int i;

    if (a0[0x49] != 0) {
        p = a0 + 0x5D;
        fill = 0xFF;
        a0[0x49] = 0;
        a0[0x46] = 0;
        *(volatile short *)(a0 + 0xE6) = 0;
        *(int *)(a0 + 0x14) = 0;
        *(int *)(a0 + 0x18) = 0;
        a0[0xE3] = 0;
        a0[0xE4] = 0;
        *(volatile short *)(a0 + 0xE6) = 0;
        a0[0xE9] = 0;
        a0[0xEA] = 0;
        *(int *)(a0 + 0x0) = 0;
        *(int *)(a0 + 0x4) = 0;
        *(int *)(a0 + 0x8) = 0;
        for (i = 5; i >= 0; i--) {
            *p = fill;
            p++;
        }
    }
}
