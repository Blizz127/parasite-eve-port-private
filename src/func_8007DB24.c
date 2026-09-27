extern int D_8009B420;
extern unsigned int D_8009B428;
extern unsigned int D_8009B42C;
extern unsigned int D_8009B424;
extern unsigned short *D_8009B3FC;

int func_8007DB24(int a0, unsigned int a1) {
    unsigned int shifted;
    register unsigned int low asm("$3");
    unsigned int d;
    if (D_8009B420 != 0) {
        d = D_8009B428;
        if (a1 % d != 0) {
            asm volatile("" : "=r"(d) : "0"(d));
            a1 = a1 + d;
            a1 = a1 & ~D_8009B42C;
        }
    }
    shifted = a1 >> D_8009B424;
    low = shifted;
    if (a0 == -2)
        goto ret_a1;
    if (a0 != -1)
        goto store;
    return (int)(low & 0xFFFFu);
ret_a1:
    return (int)a1;
store:
    D_8009B3FC[a0] = (unsigned short)shifted;
    return (int)a1;
}
