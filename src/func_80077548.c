extern unsigned int *D_80095854;
extern unsigned int *D_80095850;

int func_80077548(int a0)
{
    unsigned int *p;

    *D_80095854 = 0x10000007;
    p = D_80095850;
    if ((*p & 0xFFFFFF) != 2) {
        *p = (*D_80095854 & 0x3FFF) | 0xE1001000;
        *(volatile unsigned int *)D_80095850;
        return 0;
    }
    if ((a0 & 8) == 0) {
        return 1;
    }
    *D_80095854 = 0x9000001;
    return 2;
}
