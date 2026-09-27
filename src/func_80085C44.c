extern volatile unsigned short *D_8009B3FC;
extern char D_8001208C[];
extern char D_8001209C[];
extern char D_800120AC[];
extern void func_80071A74(char *, char *);

int func_80085C44(int a0)
{
    unsigned int i;

    if (a0 == 0 || a0 == 3) {
        D_8009B3FC[0xD5] &= ~0x40;
        i = 0;
        while (D_8009B3FC[0xD5] & 0x40) {
            if (++i > 0xF00) {
                func_80071A74(D_8001208C, D_8001209C);
                return -1;
            }
        }
    }
    if (a0 == 1 || a0 == 3) {
        D_8009B3FC[0xD5] |= 0x40;
        i = 0;
        while (!(D_8009B3FC[0xD5] & 0x40)) {
            if (++i > 0xF00) {
                func_80071A74(D_8001208C, D_800120AC);
                return -1;
            }
        }
    }
    return a0;
}
