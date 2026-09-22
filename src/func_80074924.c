extern int func_80074A28(void);

void func_80074924(unsigned char *o, int a1, int a2, int a3, int a4)
{
    unsigned char *q;
    int r;

    r = func_80074A28();
    *(short *)(o + 0) = a1;
    *(short *)(o + 2) = a2;
    *(short *)(o + 4) = a3;
    *(short *)(o + 0xC) = 0;
    *(short *)(o + 0xE) = 0;
    *(short *)(o + 0x10) = 0;
    *(short *)(o + 0x12) = 0;
    o[0x19] = 0;
    o[0x1A] = 0;
    o[0x1B] = 0;
    o[0x16] = 1;
    *(short *)(o + 6) = a4;
    if (r != 0) {
        o[0x17] = a4 < 0x121;
    } else {
        o[0x17] = a4 < 0x101;
    }
    q = o;
    asm("" : "=r"(q) : "0"(q));
    *(short *)(q + 8) = a1;
    *(short *)(q + 0xA) = a2;
    *(short *)(q + 0x14) = 10;
    q[0x18] = 0;
}
