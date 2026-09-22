extern int D_800A8020;
typedef void (*Fn)();
extern Fn func_800824F0(Fn a0);
extern Fn func_800824C8(Fn a0);
extern int func_8007F0C8(int a0, int a1, int a2, int a3, int a4);
extern void func_8007C214();
extern void func_800813E8();

int func_80081314(int a0, int a1)
{
    Fn h1;
    Fn h2;
    int r;

    if (a1 & 0x100) {
        if (a1 & 0x20) {
            D_800A8020 = 0;
        } else {
            D_800A8020 = 1;
        }
        h1 = func_800824F0(func_8007C214);
        h2 = func_800824C8(func_800813E8);
        r = func_8007F0C8(a1 & 0xFF, a0, 0x1B, 0, -1);
        if (r != 0) {
            return r;
        }
        func_800824F0(h1);
        func_800824C8(h2);
        return 0;
    }
    return func_8007F0C8(a1 & 0xFF, a0, 0x1B, 0, -1);
}
