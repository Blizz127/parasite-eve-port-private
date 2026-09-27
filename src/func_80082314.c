extern volatile int D_800B28F8;
extern int func_8007FBF0(int);
extern int func_8007F72C(void);
extern int func_800822AC(void);
extern void func_80081DF8(void);
extern void func_80080B44(int, unsigned char *);
extern int func_8007F0C8(int, unsigned char *, int, void (*)(), int);
extern void func_80082400();

int func_80082314(void)
{
    unsigned char buf[8];

    if (func_8007FBF0(0) == 2 && func_8007FBF0(1) == 0x10) {
        return 0x10;
    }
    for (;;) {
        if (func_8007F72C() == 1) {
            break;
        }
        if (func_8007F72C() == 3) {
            return 1;
        }
    }
    if (func_800822AC() != 0) {
        func_80081DF8();
    }
    func_80080B44(0x10, buf);
    D_800B28F8 = 0;
    if (func_8007F0C8(0x20, buf, 0x1B, func_80082400, 0) == 0) {
        return 2;
    }
    while (D_800B28F8 == 0) {
    }
    return D_800B28F8;
}
