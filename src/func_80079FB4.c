extern short D_8009A6EC[];
int func_80079FB4(int y, int x)
{
    int c, v, r;
    int s1, s2;
    s1 = 0;
    asm("" : "=r"(s1) : "0"(s1));
    s2 = 0;
    if (x < 0) { s1 = 1; x = -x; }
    if (y < 0) { s2 = 1; y = -y; }
    if (x != 0 || (r = 0, y != 0)) {
    if (y < x) {
        if (y & 0x7FE00000) v = y / (x >> 10);
        else v = (y << 10) / x;
        c = D_8009A6EC[v];
    } else {
        if (x & 0x7FE00000) v = x / (y >> 10);
        else v = (x << 10) / y;
        c = 0x400 - D_8009A6EC[v];
    }
    if (s1) c = 0x800 - c;
    if (s2) c = -c;
    r = c;
    }
    return r;
}
