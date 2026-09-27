extern int D_8009B390;
extern int D_8009B394;
extern int D_8009B398;
extern unsigned short *D_8009B3FC;
extern int func_80085BB4(int);

int func_80085A64(int a0)
{
    switch (a0) {
    case 0:
        D_8009B390 = 0;
        D_8009B3FC[0xD5] &= 0xFF7F;
        break;
    case 1:
        if (D_8009B394 != a0 && func_80085BB4(D_8009B398) != 0) {
            D_8009B390 = 0;
            D_8009B3FC[0xD5] &= 0xFF7F;
        } else {
            D_8009B390 = a0;
            D_8009B3FC[0xD5] |= 0x80;
        }
        break;
    }
    return D_8009B390;
}
