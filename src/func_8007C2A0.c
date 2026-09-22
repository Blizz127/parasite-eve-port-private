extern int D_800A8020;
extern unsigned char D_800A3490[];
extern int D_800A3494;
extern int func_8007AA34(unsigned char *);
extern void func_8007A930(int, int);

int func_8007C2A0(int a0)
{
    if (D_800A8020 != 0) {
        return -1;
    }
    func_8007A930(func_8007AA34(D_800A3490) + 1, a0);
    return D_800A3494;
}
