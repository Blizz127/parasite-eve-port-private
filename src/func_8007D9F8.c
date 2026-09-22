extern int D_8009B418;
extern unsigned short D_8009B414;
extern int D_8009B424;
extern void func_8007D778();
extern void func_8007D454(int, int);

int func_8007D9F8(int a0, int a1)
{
    if (D_8009B418 != 0) {
        func_8007D454(a0, a1);
        return a1;
    }
    func_8007D778(2, D_8009B414 << D_8009B424);
    func_8007D778(1);
    func_8007D778(3, a0, a1);
    return a1;
}
