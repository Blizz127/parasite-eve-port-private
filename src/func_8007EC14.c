extern unsigned char D_800B8AB0;
extern unsigned char D_800A3510;
extern unsigned char D_800A3515[];
extern unsigned char D_800A3525[];
extern unsigned char D_800A3535[];
extern unsigned char D_800A3540;
extern int D_800A3600;
extern int D_800A3604;
extern int D_800A3608;
extern int D_800A3610[];
extern int D_800A3690;
extern int func_80080940(void);
extern void func_8007E594(unsigned char *a0);
extern void func_800822BC(void);
extern void func_80081E5C(int a0);
extern void func_8007F994(void);
extern void func_8007FBCC(void (*a0)());
extern void func_8007FBD8(void (*a0)());
extern void func_8007FBE4(void (*a0)());
extern void func_8007FBC0(void (*a0)());
extern void func_8007E964();
extern void func_8007F88C();
extern void func_8007F960();
extern void func_8007F7E8();

int func_8007EC14(void)
{
    int i;
    int j;
    int k;
    int off;
    unsigned char *p;
    unsigned char *b;
    unsigned char *c;

    if (func_80080940() != 0) {
        return 0;
    }
    i = 0;
    b = &D_800B8AB0;
    __asm__ ("" : "=r"(b) : "0"(b));
    *(int *)(b + 0) = 0;
    *(int *)(b + 4) = 0;
    *(int *)(b + 8) = 0;
    __asm__ __volatile__("");
    c = &D_800A3510;
    __asm__ ("" : "=r"(c) : "0"(c));
    *(int *)(c + 0x20) = 0;
    *(int *)(c + 0x10) = 0;
    *(int *)(c + 0x00) = 0;
    c[0x24] = 0;
    c[0x14] = 0;
    c[0x04] = 0;
    do {
        D_800A3515[i] = 0;
        D_800A3525[i] = 0;
        D_800A3535[i] = 0;
        i++;
    } while (i < 8);
    j = 0;
    p = &D_800A3540;
    do {
        func_8007E594(p);
        j++;
        p += 0x18;
    } while (j < 8);
    k = 7;
    off = 0x70;
    D_800A3604 = 0;
    D_800A3600 = 0;
    D_800A3608 = 0;
    do {
        *(int *)((unsigned char *)D_800A3610 + off) = 0;
        k--;
        off -= 0x10;
    } while (k >= 0);
    D_800A3690 = 0;
    func_8007F994();
    func_8007FBCC(func_8007E964);
    func_8007FBD8(func_8007F88C);
    func_8007FBE4(func_8007F960);
    func_8007FBC0(func_8007F7E8);
    func_800822BC();
    func_80081E5C(0);
    return 1;
}
