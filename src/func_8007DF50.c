extern int D_8009B4AC;
extern void func_8007E2A4(void);
extern void func_80072714(void);
extern void func_8007E22C(void);
extern void func_80072724(void);
extern void func_80073C74(int);
extern void func_8007E048(void);
extern void func_8007E1A4(int, int, int, int);

void func_8007DF50(int a0, int a1, int a2, int a3)
{
    func_8007E2A4();
    func_80072714();
    func_8007E22C();
    func_80072724();
    func_80073C74(0);
    func_8007E048();
    func_8007E1A4(a0, a1, a2, a3);
    D_8009B4AC = 1;
}
