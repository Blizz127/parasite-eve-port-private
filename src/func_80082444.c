extern int D_800B28F8;
extern char D_8001205C;
extern void func_80080AE4(char *a0, int a1);
extern int func_80071A04(char *a0, char *a1, int a2);
extern void func_80081DF8(void);

void func_80082444(unsigned char a0)
{
    char buf[8];
    int v;

    if (a0 == 1) {
        func_80080AE4(&buf[0], 2);
        v = func_80071A04(&buf[1], &D_8001205C, 5);
        if (v == 0) {
            v = 4;
        } else {
            v = 2;
        }
    } else {
        v = 2;
    }
    D_800B28F8 = v;
    func_80081DF8();
}
