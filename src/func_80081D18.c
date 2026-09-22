extern void func_80080B44(int a0, int *a1);
extern void func_80080E34(int *a0, int a1, int a2, int a3);
extern int func_800811E4(int a0);

int func_80081D18(int a0, int a1, int a2)
{
    int buf[2];
    int r;

    func_80080B44(a1, buf);
    func_80080E34(buf, a0, a2, 0x80);
    do {
        r = func_800811E4(0);
    } while (r > 0);
    return r == 0;
}
