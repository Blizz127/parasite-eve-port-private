extern int D_8009B770;
extern unsigned char *D_8009B758;

int func_80082574(int a0)
{
    if (D_8009B770 != 0) {
        return D_8009B758[(a0 >> 4) * 240 + 0xE8] == 8;
    }
    return 0;
}
