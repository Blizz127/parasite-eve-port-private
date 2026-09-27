extern int (*D_8009B740)();
extern void func_80083D9C();
extern void func_80083DF0();

int func_80083D04(unsigned char *a0, int a1, int a2) {
    register int s3 asm("$19");
    register unsigned char cur asm("$4");
    register int m asm("$3");

    s3 = a1;
    if (D_8009B740(a0) != 0) {
        return 0;
    }
    cur = a0[0xE4];
    a0[0x46] = 1;
    *(void **)(a0 + 0x14) = func_80083D9C;
    *(void **)(a0 + 0x18) = func_80083DF0;
    m = s3 & 255;
    a0[0x51] = a1;
    a0[0x52] = a2;
    a0[0x53] = ((m ^ cur) == 0);
    return 1;
}
