struct S4 { char b[4]; };

extern int D_800BE9E4;
extern unsigned char *D_800C0DC8;
extern unsigned char D_800A3490[];
extern int D_800A3494;
extern int D_800BE998;
extern void (*D_800B0CC8)();
extern int D_800B89F4;

void func_8007C214(void)
{
    unsigned char *p;
    void (*f)();

    p = D_800C0DC8 + (D_800BE9E4 << 5);
    *(short *)p = 2;
    *(struct S4 *)D_800A3490 = *(struct S4 *)(p + 0x1C);
    D_800A3494 = *(int *)(p + 8);
    D_800BE9E4 = D_800BE998;
    f = D_800B0CC8;
    if (f != 0) {
        f();
    }
    D_800B89F4 = 0;
}
