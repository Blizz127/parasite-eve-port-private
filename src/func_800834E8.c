extern int *D_8009B784;
extern unsigned short *D_8009B788;
extern int func_80084FE4();

int func_800834E8(void)
{
    int *q;
    unsigned short *p;

    q = D_8009B784;
    p = D_8009B788;
    *q = -0x81;
    if (p[2] & 0x80) {
        do {
            if (func_80084FE4() != 0) {
                return 0;
            }
        } while (D_8009B788[2] & 0x80);
    }
    D_8009B788[5] |= 0x10;
    return 1;
}
