extern int (*D_8009B740)();
extern void func_8008389C();
extern void func_80083944();

int func_800837C8(unsigned char *a0, int a1)
{
    register int r asm("$2");
    int n;
    int four;
    int sz;
    int e3;
    int e9;

    if (a1 == 0) {
        goto zero;
    }
    if (*(int *)(a0 + 4) != 0) {
        return 0;
    }
    if (D_8009B740() == 0) {
        goto body;
    }
zero:
    r = 0;
    goto end;
body:
    r = 1;
    four = 4;
    n = (a1 + 3) >> 2;
    a0[0x49] = four;
    a0[0x46] = 1;
    *(void **)(a0 + 0x14) = func_8008389C;
    e3 = a0[0xE3];
    *(void **)(a0 + 0x18) = func_80083944;
    e9 = a0[0xE9];
    sz = n << 2;
    *(int *)(a0 + 0x0) = sz;
    a0[0x47] = 0;
    sz = sz + (((e3 + 1) >> 1) << 2);
    *(int *)(a0 + 0x4) = sz;
    *(int *)(a0 + 0x8) = sz + ((e9 * 5 + 3) & 0xFFC);
end:
    return r;
}
