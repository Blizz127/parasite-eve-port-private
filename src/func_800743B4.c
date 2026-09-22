extern void (*D_8009568C[8])();
extern int *D_800956B0;
extern int D_800956AC;
extern void func_800744A4(void *a0, int a1);
extern void func_80073CC4(int a0, void *a1);
extern void func_8007440C();
extern void func_80074478();

void *func_800743B4(void)
{
    *D_800956B0 = 0x107;
    D_800956AC = 0;
    func_800744A4(D_8009568C, 8);
    func_80073CC4(0, func_8007440C);
    return func_80074478;
}
