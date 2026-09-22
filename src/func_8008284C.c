extern unsigned char *(*D_8009B738)();

int func_8008284C(int a0, int a1, int a2)
{
    unsigned char *b;
    unsigned char *p;

    b = D_8009B738(a0);
    if (a1 < 0) {
        return b[0xEA];
    }
    if (a1 >= b[0xEA]) {
        return 0;
    }
    p = *(unsigned char **)(b + 8) + a1 * 8;
    if (a2 < 0) {
        return p[0];
    }
    if (a2 >= p[0]) {
        return 0;
    }
    return (*(unsigned char **)(p + 4))[a2];
}
