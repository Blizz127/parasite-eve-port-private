extern void (*D_8009B728)(unsigned char *a0);

void func_80084EB0(unsigned char *a0)
{
    int st;
    unsigned int c;

    *(int *)(a0 + 0x4C) = *(int *)(a0 + 0x4C) + 1;
    st = a0[0x46];
    if (st == 0) {
        goto tail;
    }
    if (st == 1) {
        c = a0[0x4A];
        if (c < 2) {
            goto bump;
        }
        a0[0x49] = 2;
        a0[0x46] = 0xFF;
        return;
    }
    c = a0[0x4A];
    if (c < 4) {
bump:
        a0[0x4A] = c + 1;
        return;
    }
    if (a0[0x49] == 0) {
        goto tail;
    }
    D_8009B728(a0);
tail:
    if (**(unsigned char **)(a0 + 0x3C) != 0xF3) {
        **(unsigned char **)(a0 + 0x30) = 0xFF;
        (*(unsigned char **)(a0 + 0x30))[1] = 0;
        a0[0xE8] = 0;
    }
}
