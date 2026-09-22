extern void func_8007506C(short *a0, int a1);
extern int func_80077A64(int a0, int a1, int a2, int a3);

unsigned short func_80074774(int a0, int a1, int a2, int a3, int a4, int a5, int a6)
{
    short buf[4];

    buf[0] = a3;
    buf[3] = a6;
    buf[1] = a4;
    switch (a1) {
    case 0:
        buf[2] = a5 / 4;
        break;
    case 1:
        buf[2] = a5 / 2;
        break;
    case 2:
        buf[2] = a5;
        break;
    }
    func_8007506C(buf, a0);
    return func_80077A64(a1, a2, a3, a4);
}
