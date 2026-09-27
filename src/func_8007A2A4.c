extern void func_80072714(void);
extern void func_80072724(void);
extern void func_800824F0(int);
extern void func_800824C8(int);
extern void func_8007A8EC(int);
extern void func_8007A4BC(int);
extern int D_8009AFD8;
extern unsigned char *D_8009AF1C;
extern unsigned char *D_8009AF28;

void func_8007A2A4(void)
{
    func_80072714();
    if (D_8009AFD8 == 1) {
        func_800824F0(0);
        func_800824C8(0);
    } else {
        func_8007A8EC(0);
        func_8007A4BC(0);
    }
    *D_8009AF1C = 0;
    *D_8009AF28 = 0;
    func_80072724();
}
