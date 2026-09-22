extern volatile int D_800A36A8;
extern volatile int D_800A36A4;
extern volatile int D_800A36A0;
extern void (*D_8009AFB4)();
extern void (*D_8009AFB8)();
extern int D_8009AFD8;
extern int D_8009B554;

extern void func_8007BBFC(void);
extern void func_8007BAC0(void);
extern void func_8007FA2C(void);
extern void func_800812F4(int);
extern void func_80080164();
extern void func_80080778();
extern void func_8007FE24();
extern void func_80073D58(int, void (*)());

void func_8007F994(void)
{
    func_8007BBFC();
    func_8007BAC0();
    D_800A36A8 = 0;
    D_800A36A4 = D_800A36A8;
    D_800A36A0 = 0;
    func_8007FA2C();
    func_800812F4(0);
    D_8009AFB4 = func_80080164;
    D_8009AFB8 = func_80080778;
    func_80073D58(0, func_8007FE24);
    D_8009AFD8 = 1;
    D_8009B554 = 1;
}
