void func_80083D9C(unsigned char *a0)
{
    int t;

    t = a0[0x46];
    switch (t) {
    case 2:
        a0[0x36] = 0x44;
        *(unsigned char **)(a0 + 0x2C) = a0 + 0x51;
        a0[0x35] = t;
        break;
    case 3:
        a0[0x36] = 0x4D;
        *(unsigned char **)(a0 + 0x2C) = a0 + 0x5D;
        a0[0x35] = 6;
        break;
    }
}
