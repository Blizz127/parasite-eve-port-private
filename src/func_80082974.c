extern int (*D_8009B738)();
extern void func_800835A4(int a0, int a1, int a2);

void func_80082974(int a0, int a1, int a2)
{
    func_800835A4(D_8009B738(a0), a1, a2);
}
