extern unsigned int *volatile D_8009B40C;

void func_8007DC04(int a0)
{
    *D_8009B40C &= 0xFFF8FFFF;
    if (a0 != 0) {
        *D_8009B40C |= 0x30000;
    } else {
        *D_8009B40C |= 0x50000;
    }
}
