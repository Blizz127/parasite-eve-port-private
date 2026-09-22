extern int (*D_8009B738)();
extern void func_80083D04(int a0, unsigned char a1, unsigned char a2);

void func_8008292C(int a0, unsigned char a1, unsigned char a2)
{
    func_80083D04(D_8009B738(a0), a1, a2);
}
