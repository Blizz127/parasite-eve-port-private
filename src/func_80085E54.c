extern int D_8009B434;
extern int D_8009B430;
extern void func_8007D9F8(int a0, unsigned int a1);

unsigned int func_80085E54(int a0, unsigned int a1)
{
    if (a1 > 0x7EFF0) {
        a1 = 0x7EFF0;
    }
    func_8007D9F8(a0, a1);
    if (D_8009B434 == 0) {
        D_8009B430 = 0;
    }
    return a1;
}
