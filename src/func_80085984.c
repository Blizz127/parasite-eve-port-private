extern int D_8009B3EC;
extern volatile int D_8009B434;
extern volatile int D_8009B438;
extern int D_8009B384;
void func_80072714(void);
void func_8007DD14(int a);
void func_800857E4(int a);
void func_800857F4(int a);
void func_80072724(void);

void func_80085984(void) {
    register int z asm("$4");

    if (D_8009B3EC == 1) {
        D_8009B3EC = 0;
        func_80072714();
        z = 0;
        D_8009B434 = 0;
        D_8009B438 = 0;
        func_8007DD14(z);
        func_800857E4(D_8009B384);
        func_800857F4(D_8009B384);
        func_80072724();
    }
}
