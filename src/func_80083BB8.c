extern int (*D_8009B740)();
extern void func_80083C20();
extern void func_80083C3C();

int func_80083BB8(unsigned char *a0, int a1)
{
    if (D_8009B740(a0) != 0) {
        return 0;
    }
    a0[0x46] = 1;
    *(void **)(a0 + 0x14) = func_80083C20;
    *(int *)(a0 + 0x20) = a1;
    *(void **)(a0 + 0x18) = func_80083C3C;
    return 1;
}
