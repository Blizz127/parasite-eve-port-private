extern unsigned short D_8009B414;
extern int D_8009B424;
extern void func_8007D778();

int func_8007DA7C(int a0, int a1)
{
    func_8007D778(2, D_8009B414 << D_8009B424);
    func_8007D778(0);
    func_8007D778(3, a0, a1);
    return a1;
}
