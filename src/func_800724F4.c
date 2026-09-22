void func_800724F4(unsigned char *a, unsigned char *b, unsigned int n)
{
    register unsigned int i asm("$7");
    register unsigned char *p asm("$3");
    register unsigned char *q asm("$4");
    unsigned char t;
    unsigned char u;

    i = 0;
    if (n != 0) {
        do {
            p = a + i;
            q = b + i;
            t = *p;
            u = *q;
            *p = u;
            *q = t;
            i++;
        } while (i < n);
    }
}
