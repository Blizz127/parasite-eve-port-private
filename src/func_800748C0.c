extern void func_8007506C(int a0, int a1);
extern int func_80077AA4(int a0, int a1);

unsigned short func_800748C0(int a0, int a1, int a2)
{
    short buf[4];

    buf[2] = 0x10;
    buf[0] = a1;
    buf[1] = a2;
    buf[3] = 1;
    func_8007506C((int)buf, a0);
    return func_80077AA4(a1, a2);
}
