extern void func_80083E50(unsigned char *, int);
extern void func_800835C0(unsigned char *);

int func_80084B78(unsigned char *a0)
{
    if (**(unsigned char **)(a0 + 0x3C) == 0xF3 && a0[0xE8] == 0) {
        func_80083E50(a0, 0);
        return 0;
    }
    switch (a0[0x46]) {
    case 1:
        func_80083E50(a0, 1);
        break;
    case 0xFE:
        func_80083E50(a0, 0);
        break;
    case 0:
    case 0xFF:
        break;
    default:
        if (*(void (**)())(a0 + 0x14) != 0) {
            (*(void (**)())(a0 + 0x14))(a0);
        } else {
            func_800835C0(a0);
        }
        break;
    }
    return 0;
}
