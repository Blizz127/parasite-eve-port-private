extern void func_80083E70();
extern void func_80083E84(unsigned char *a0, unsigned char a1);
extern void func_80083EC4(unsigned char *a0, unsigned char a1);

void func_800835C0(unsigned char *a0)
{
    switch (a0[0x46]) {
    case 2:
        func_80083E70();
        break;
    case 3:
        func_80083E84(a0, a0[0xE4]);
        break;
    case 4:
        func_80083EC4(a0, a0[0x47]);
        break;
    }
}
