extern int D_8009B6EC[9];
extern void func_800824C8(int a0);
extern void func_800824DC(int a0);
extern int func_8007F778();
extern void func_80082204();

void func_8008214C(unsigned char a0, int a1)
{
    int *p;
    unsigned int k;
    void (*f)(unsigned int a0, int a1, int a2);

    p = &D_8009B6EC[7];
    asm ("" : "=r" (p) : "0" (p));
    if (p[0] == 0) {
        return;
    }
    if (p[1] == 0) {
        return;
    }
    k = a0;
    if (k == 2) {
        if (func_8007F778() != 0) {
            return;
        }
        func_80082204();
        return;
    }
    func_800824C8(p[-2]);
    func_800824DC(p[-1]);
    f = (void (*)(unsigned int, int, int))p[-5];
    p[1] = 0;
    if (f != 0) {
        f(k, a1, 0);
    }
}
