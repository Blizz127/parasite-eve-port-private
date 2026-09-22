extern void func_80083E84(unsigned char *a0, unsigned char a1);
extern void func_80083EA4(unsigned char *a0, unsigned char a1);
extern void func_80083EC4(unsigned char *a0, unsigned char a1);
extern void func_80083EE4();

void func_8008389C(unsigned char *a0)
{
    switch (a0[0x46]) {
    case 2:
        func_80083E84(a0, a0[0x47]);
        break;
    case 3:
        func_80083EA4(a0, a0[0x47]);
        break;
    case 4:
        if (a0[0x48] == 0) {
            func_80083EC4(a0, a0[0x47]);
        } else {
            func_80083EE4();
        }
        break;
    }
}
