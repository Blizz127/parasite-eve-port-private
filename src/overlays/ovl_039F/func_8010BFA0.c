void func_8010C1EC(unsigned int *a0, int a1);

void func_8010BFA0(unsigned int *a0, int a1)
{
    if (a1 & 1) {
        *a0 &= 0xF7FFFFFF;
    } else {
        *a0 |= 0x08000000;
    }
    if (a1 & 2) {
        *a0 |= 0x02000000;
    } else {
        *a0 &= 0xFDFFFFFF;
    }
    func_8010C1EC(a0, (unsigned short)*a0);
}
