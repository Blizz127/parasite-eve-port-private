extern int D_8009B3EC;
extern int D_8009B384;
extern void func_8007D614();
extern void func_80072714(void);
extern void func_8007DD14(void (*)());
extern int func_800726E4(unsigned int, int, int, int);
extern void func_80072704(int);
extern void func_80072724(void);

void func_8007D15C(void)
{
    int r;

    if (D_8009B3EC == 0) {
        D_8009B3EC = 1;
        func_80072714();
        func_8007DD14(func_8007D614);
        r = func_800726E4(0xF0000009, 0x20, 0x2000, 0);
        D_8009B384 = r;
        func_80072704(r);
        func_80072724();
    }
}
