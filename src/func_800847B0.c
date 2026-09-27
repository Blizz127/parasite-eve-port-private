int func_800847B0(unsigned char *a0) {
    int idx;
    unsigned char tag;
    unsigned char *p;
    int c;

    idx = (int)a0[0x45] - 3;
    tag = a0[0x36];
    if (tag == 0) {
        goto zero;
    }
    if (tag == 0x4D) {
        goto fourd;
    }
    goto other;

zero:
    if (!(idx < 6 && (a0 + idx)[0x57] == 0)) {
        if (idx < (int)a0[0x34]) {
            p = *(unsigned char **)(a0 + 0x28);
            p += idx;
            goto byte;
        }
    }
    return 0;

byte:
    c = *p;
    asm volatile("" : "=r"(c) : "0"(c));
    return c;

fourd:
    if (idx < (int)a0[0x35]) {
        p = *(unsigned char **)(a0 + 0x2C);
        p += idx;
        goto byte;
    }
    return 255;

other:
    if (idx < (int)a0[0x35]) {
        p = *(unsigned char **)(a0 + 0x2C);
        asm volatile("" : "=r"(p) : "0"(p));
        return p[idx];
    }
    return 0;
}
