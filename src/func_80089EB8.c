extern unsigned char *D_8009B3FC;

int func_80089EB8(int a0)
{
    register int t asm("$2");
    register int v asm("$5");
    unsigned short *p;
    unsigned int w;

    t = a0;
    v = 0;
    if (t >= 0) {
        v = t;
        if (v >= 0x40) {
            v = 0x3F;
        }
    }
    p = (unsigned short *)(D_8009B3FC + 0x1AA);
    t = (v & 0x3F) << 8;
    w = *p & 0xC0FF;
    w |= t;
    *p = w;
    return v;
}
