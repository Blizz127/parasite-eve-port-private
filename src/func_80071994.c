unsigned char *func_80071994(unsigned char *a0)
{
    register unsigned char *p asm("$2");
    register int d asm("$3");
    if (*(int *)(a0 + 4) & 8) {
        d = *(int *)(a0 + 8);
        p = a0 + 8;
        p = p + d;
    } else {
        p = a0 + 8;
    }
    return p + 12;
}
