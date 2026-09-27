extern int D_8009AFB4;
extern unsigned char D_8009AFC4;
extern int D_8009AF2C[];
extern int func_8007B558(int, unsigned char *, unsigned char *, int);
extern int func_8007B010(int, unsigned char *);

int func_8007A740(unsigned char a0, unsigned char *a1, unsigned char *a2)
{
    register int i asm("$16");
    int err;
    register int saved asm("$21");
    int *slot;
    int one;
    int neg1;
    int c;

    i = 3;
    one = 1;
    c = a0;
    saved = D_8009AFB4;
    {
        register int *b asm("$3") = D_8009AF2C;
        slot = &b[c];
    }
    neg1 = -1;
    do {
        D_8009AFB4 = 0;
        if (c != one && (D_8009AFC4 & 0x10)) {
            func_8007B558(1, 0, 0, 0);
        }
        if (a1 == 0 || *slot == 0 || func_8007B558(2, a1, a2, 0) == 0) {
            D_8009AFB4 = saved;
            if (func_8007B558(a0, a1, a2, 0) == 0) {
                err = 0;
                goto done;
            }
        }
    } while (--i != neg1);
    D_8009AFB4 = saved;
    err = -1;
done:
    if (err != 0) {
        return 0;
    }
    return func_8007B010(0, a2) == 2;
}
