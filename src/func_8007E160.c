extern unsigned int *D_8009B4B4;

int func_8007E160(void)
{
    unsigned int *p;
    int one;

    p = D_8009B4B4;
    if ((p[1] & 1) == 0) {
        return 0;
    }
    if ((p[0] & 1) != 0) {
        one = 1;
        asm volatile("" : "=r"(one) : "0"(one));
        return one;
    }
    return 0;
}
