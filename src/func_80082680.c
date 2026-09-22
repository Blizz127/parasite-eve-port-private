extern unsigned char *(*D_8009B738)();

int func_80082680(int a0, int a1, int a2)
{
    unsigned char *b;

    b = D_8009B738(a0);
    switch (a1) {
    case 1:
        return b[0xE8];
    case 2:
        return *(unsigned short *)(b + 0xE6);
    case 3:
        return b[0xE4];
    case 4:
        if (a2 < 0) {
            return b[0xE3];
        }
        if (a2 >= b[0xE3]) {
            return 0;
        }
        return (*(unsigned short **)b)[a2];
    case 0x64:
        return *(int *)(b + 0x4C);
    }
    return 0;
}
