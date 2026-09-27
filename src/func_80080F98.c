extern int D_8009B6C8;
extern void (*D_8009B6D0)(int, int);
extern int func_80073A44(int);
extern int func_80080B04(int, int);
extern int func_80080AE4(int, int);
extern void func_80081268(void);

void func_80080F98(int a0, int a1)
{
    register int *p asm("$16");
    int vs;
    int count;
    void (*cb)(int, int);
    int arg;

    vs = func_80073A44(-1);
    p = &D_8009B6C8;
    p[0] = vs;
    if (p[-4] & 1) {
        if (p[-5] > 0) {
            func_80080B04(p[-6], p[-7]);
            p[-3] = a1;
            return;
        }
        func_80081268();
        cb = D_8009B6D0;
        if (cb == 0) {
            return;
        }
        count = ((volatile int *)p)[-5];
        asm volatile("" : "=r"(count) : "0"(count));
        goto tail;
    }
    if (p[-5] > 0) {
        int a;
        int b;
        int c;

        func_80080AE4(p[-6], p[-7]);
        a = p[-7];
        b = p[-6];
        c = p[-5];
        p[-6] = b + (a << 2);
        p[-5] = c - 1;
    }
    vs = func_80073A44(-1);
    if (p[-1] + 0x4B0 < vs) {
        p[-5] = -1;
    }
    if (p[-5] != 0) {
        vs = func_80073A44(-1);
        if (p[-1] + 0x4B0 >= vs) {
            return;
        }
    }
    func_80081268();
    cb = D_8009B6D0;
    if (cb == 0) {
        return;
    }
    a0 = 2;
    count = ((volatile int *)p)[-5];
tail:
    asm volatile("" : "=r"(count) : "0"(count));
    arg = a0 & 0xFF;
    if (count < 0) {
        a0 = 5;
        asm volatile("" : "=r"(a0) : "0"(a0));
        arg = a0 & 0xFF;
    }
    cb(arg, a1);
}
