extern unsigned char D_800A3540[];
extern int D_800A3600;
extern int D_800A3604;
extern int D_800A3608;
extern void func_800808BC(void);
extern void func_80081DF8(void);
extern void func_80080930(void);

void func_8007F69C(void)
{
    register unsigned char *e asm("$4");
    register unsigned char *p asm("$2");
    register int i asm("$5");
    register int j asm("$3");

    func_800808BC();
    i = 0;
    e = D_800A3540;
    D_800A3608 = 0;
    D_800A3604 = 0;
    D_800A3600 = 0;
    for (; i < 8; i++) {
        j = 3;
        p = e + 3;
        *(int *)e = 0;
        e[4] = 0;
        do {
            p[5] = 0;
            j--;
            p--;
        } while (j >= 0);
        *(int *)(e + 0xC) = 0;
        *(int *)(e + 0x10) = 0;
        *(int *)(e + 0x14) = 0;
        e += 0x18;
    }
    func_80081DF8();
    func_80080930();
}
