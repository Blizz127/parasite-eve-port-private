extern int D_800A34B0;
extern int D_800A34B4;
extern int D_800A34BC;
extern void func_8007E0F8(void);
extern void func_8007E160(void);
extern void func_80072714(void);
extern void func_80072724(void);
extern void func_8007E1F4(int, int *);
extern void func_8007E1E4(int, int *);

int func_8007E048(void) {
    int *p;
    int *q;
    func_80072714();
    p = &D_800A34B4;
    q = p - 1;
    p[0] = (int)func_8007E0F8;
    p[1] = (int)func_8007E160;
    D_800A34B0 = 0;
    D_800A34BC = 0;
    func_8007E1F4(1, q);
    func_8007E1E4(1, q);
    func_80072724();
    return 1;
}
