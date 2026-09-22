extern unsigned short D_800945E4;
extern unsigned short *D_80095670;
extern unsigned short *D_80095674;
extern int *D_80095678;
extern void func_80072714(void);
extern void func_80074394(void);

void *func_80074218(void)
{
    unsigned char *p = (unsigned char *)&D_800945E4;
    register unsigned short *q asm("$2");
    register int *r asm("$4");

    asm volatile("" : "=r"(p) : "0"(p));
    if (*(unsigned short *)p == 0) {
        return 0;
    }
    func_80072714();
    q = D_80095674;
    r = D_80095678;
    *(unsigned short *)(p + 0x32) = *q;
    {
        int v = *r;
        unsigned short *u = D_80095670;
        *(int *)(p + 0x34) = v;
        *q = 0;
        asm("" : "=r"(q) : "0"(q));
        *u = *q;
    }
    *D_80095678 &= 0x77777777;
    func_80074394();
    *(short *)p = 0;
    return p;
}
