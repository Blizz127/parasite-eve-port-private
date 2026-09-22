extern int D_8009B6AC[7];
extern void (*D_8009B6D0)(int a0, int a1);
extern int func_80073A44(int a0);
extern void func_80081268();

void func_80081110(void)
{
    int *p;
    int a;
    int b;
    int c;
    int t;
    int arg;

    p = &D_8009B6AC[1];
    asm ("" : "=r" (p) : "0" (p));
    a = p[-1];
    b = p[0];
    c = p[1];
    p[0] = b + (a << 2);
    p[1] = c - 1;
    t = func_80073A44(-1);
    if (p[5] + 0x4B0 < t) {
        p[1] = -1;
    }
    if (p[1] != 0) {
        t = func_80073A44(-1);
        if (p[5] + 0x4B0 >= t) {
            return;
        }
    }
    func_80081268();
    if (D_8009B6D0 != 0) {
        arg = 2;
        if (p[1] < 0) {
            arg = 5;
        }
        D_8009B6D0(arg, p[3]);
    }
}
