extern int D_8009B554;
extern int D_8009AFB4;
extern int D_8009AFB8;
extern void func_8007B9EC(void);
extern void func_80073D58(int, int);

void func_8007FB04(void)
{
    D_8009B554 = 0;
    func_8007B9EC();
    D_8009AFB4 = 0;
    D_8009AFB8 = 0;
    func_80073D58(0, 0);
}
