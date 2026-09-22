extern int (*D_8009B73C)();
extern int D_8009B7A4;
extern void func_800830DC(int a0, int a1);

void func_80083F04(int a0)
{
    D_8009B7A4 = D_8009B73C(a0);
    func_800830DC(a0, -2);
}
