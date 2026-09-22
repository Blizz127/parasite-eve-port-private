extern int (*D_8009B728)();

int func_80083DF0(unsigned char *a0)
{
    if (a0[0x53] != 0) {
        if (a0[0x46] == 2) {
            return 1;
        }
        a0[0x46] = 0xFE;
    } else {
        D_8009B728(a0);
    }
    return 0;
}
