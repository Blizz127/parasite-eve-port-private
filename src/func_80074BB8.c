extern unsigned char D_8009574E[];
extern void (*D_80095748)(unsigned char *, unsigned char, unsigned char, unsigned char);
extern unsigned char D_80011814[];

unsigned char func_80074BB8(unsigned char a0)
{
    register unsigned char *p asm("$3");
    register unsigned char old asm("$16");

    p = D_8009574E;
    old = p[0];
    p[0] = a0;
    asm("" : "=r"(p) : "0"(p));
    if (a0 != 0) {
        D_80095748(D_80011814, p[0], p[-2], p[1]);
    }
    return old;
}
