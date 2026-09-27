extern unsigned char D_8010CBFC[];

void func_8010BD4C(unsigned char *dst)
{
    unsigned char *src;
    unsigned char *d;
    int off;
    int c;
    unsigned char b;
    int i;

    off = 0;
    src = D_8010CBFC;
    d = dst;
    do {
        b = *src++;
        c = b;
        if (b < 0xF0) {
            if (off != 0) {
                for (; c >= 0; c--) {
                    *d = *(d - off);
                    d++;
                }
            } else {
                for (; c >= 0; c--) {
                    *d = *src++;
                    d++;
                }
            }
        } else {
            off = 0;
            if (c != 0xF0) {
                off = ((c << 8) | *src++) - 0xF0FF;
            }
        }
    } while (off != 0xF00);
    for (i = 4; i < 0x8800; i++) {
        ((unsigned short *)dst)[i] ^= ((unsigned short *)dst)[i - 4];
    }
}
