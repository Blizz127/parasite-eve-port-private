extern char D_800118E0;
extern void func_80074E28(char *name, int r);
extern int func_80073A44(int mode);
extern volatile unsigned int *D_80095860;
extern int D_80095888;
extern int D_8009588C;
extern volatile unsigned int *D_80095854;
extern int func_80077404(void);
extern void func_80073CF4(int slot, void (*fn)());
extern void func_80077A00(void);
extern unsigned int *D_80095744;

int func_800776D4(int a0, int a1) {
    func_80074E28(&D_800118E0, a0);
    D_80095888 = func_80073A44(-1) + 240;
    D_8009588C = 0;
    while ((*D_80095860 & 0x01000000) != 0 || (*D_80095854 & 0x04000000) == 0) {
        if (func_80077404() != 0) {
            return -1;
        }
    }
    func_80073CF4(2, func_80077A00);
    ((void (*)(int, int))D_80095744[7])(a0, a1);
    return 0;
}
