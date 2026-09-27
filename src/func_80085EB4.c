extern volatile unsigned short D_8009B414;
extern int D_8009B424;
extern int func_8007DB24(int a0, int a1);

int func_80085EB4(int a0)
{
    if ((unsigned int)(a0 - 0x1010) > 0x7EFE8) {
        return 0;
    }
    D_8009B414 = func_8007DB24(-1, a0);
    return D_8009B414 << D_8009B424;
}
