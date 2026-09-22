extern unsigned char *D_8009B27C;
extern unsigned char *D_8009B284;
extern unsigned char *D_8009B288;
extern unsigned int *D_8009B28C;
extern unsigned char D_8009B294;

void func_8007B9EC(void)
{
    register unsigned char *p asm("$3");

    *D_8009B27C = 1;
    while ((*D_8009B288 & 7) != 0) {
        *D_8009B27C = 1;
        *D_8009B288 = 7;
        *D_8009B284 = 7;
    }
    p = &D_8009B294;
    asm volatile("" : "=r"(p) : "0"(p));
    p[2] = 0;
    asm("" : "=r"(p) : "0"(p));
    p[1] = p[2];
    {
        unsigned char *r = D_8009B27C;
        p[0] = 2;
        *r = 0;
    }
    *D_8009B288 = 0;
    *D_8009B28C = 0x1325;
}
