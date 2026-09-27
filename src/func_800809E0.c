extern char D_80011D74;
extern char D_80011D94;
extern char D_80011DB0;
extern char D_80011DD8;
extern char D_80011DF8;
extern char D_80011E0C;
extern char D_80011E3C;
extern char D_80011E44;
extern char D_80011E4C;
extern int D_800A36A0;
extern int D_800A36A4;
extern int D_800A36A8;
extern unsigned char D_8009B574;
extern void func_80071A74();
extern int func_800822AC(void);

void func_800809E0(void)
{
    unsigned char *p;
    char *s;

    func_80071A74(&D_80011D74);
    p = &D_8009B574;
    __asm__ ("" : "=r"(p) : "0"(p));
    func_80071A74(&D_80011D94, *(int *)(p + 0), *(int *)(p + 4), *(int *)(p + 8));
    {
        int b0 = p[-0x1C];
        int b1 = p[-0x1B];
        int b2 = p[-0x1A];
        int b3 = p[-0x19];
        int b4 = p[-0x18];
        func_80071A74(&D_80011DB0, b0, b1, b2, b3, b4);
    }
    func_80071A74(&D_80011DD8, *(int *)(p + 0x24));
    func_80071A74(&D_80011DF8, *(int *)(p + 0x20));
    func_80071A74(&D_80011E0C, D_800A36A0, D_800A36A4, D_800A36A8);
    if (func_800822AC() != 0) {
        s = &D_80011E3C;
    } else {
        s = &D_80011E44;
    }
    func_80071A74(&D_80011E4C, s);
}
