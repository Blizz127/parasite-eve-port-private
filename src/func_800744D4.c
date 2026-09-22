extern void func_8007474C(void *a0, int a1);
extern void func_80073CC4(int a0, void *a1);
extern void func_80074520();
extern void func_800746A0();
extern unsigned char D_800956C0[];
extern int *D_800956BC;

void *func_800744D4(void)
{
    func_8007474C(D_800956C0, 8);
    *D_800956BC = 0;
    func_80073CC4(3, func_80074520);
    return func_800746A0;
}
