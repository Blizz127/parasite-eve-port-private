extern volatile unsigned int *D_80094574;
extern volatile unsigned int *D_80094578;
extern volatile unsigned int D_8009457C;
extern int D_80094580;
extern volatile int D_800956AC;
extern void func_80073BBC(int a0, int a1);

int func_80073A44(int a0)
{
    volatile unsigned int t;
    unsigned int st;
    int r;
    register int v asm("$2");
    int w;

    st = *D_80094574;
    do {
        t = *D_80094578;
    } while (t != *D_80094578);
    r = (t - D_8009457C) & 0xFFFF;
    if (a0 < 0) {
        return D_800956AC;
    }
    if (a0 == 1) {
        return r;
    }
    if (a0 > 0) {
        v = D_80094580 - 1;
        v += a0;
    } else {
        v = D_80094580;
    }
    if (a0 > 0) {
        w = a0 - 1;
    } else {
        w = 0;
    }
    func_80073BBC(v, w);
    st = *D_80094574;
    func_80073BBC(D_800956AC + 1, 1);
    if (st & 0x400000) {
        while (((st ^ *D_80094574) & 0x80000000) == 0) {
        }
    }
    D_80094580 = D_800956AC;
    do {
        D_8009457C = *D_80094578;
    } while (D_8009457C != *D_80094578);
    return r;
}
