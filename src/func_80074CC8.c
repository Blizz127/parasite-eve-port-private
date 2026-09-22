extern int (*D_80095748)(char *, int);
extern char D_80011854;
extern unsigned char D_8009574E;

int func_80074CC8(int a0)
{
    register unsigned char *p asm("$16") = &D_8009574E;
    register int x asm("$17");
    int old;

    __asm__ __volatile__("" : "=r"(p) : "0"(p));
    x = a0;
    if (*p >= 2) {
        D_80095748(&D_80011854, x);
    }
    old = *(int *)(p + 0xA);
    *(int *)(p + 0xA) = x;
    return old;
}
