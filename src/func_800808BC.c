extern int D_8009B554;
extern int D_8009B574;
extern void func_8007B9EC(void);

void func_800808BC(void)
{
    register int *p asm("$16") = &D_8009B554;
    int *q;
    int v;

    asm volatile("" : "=r"(p) : "0"(p));
    p[0] = 0;
    func_8007B9EC();
    if (p[8] != 2) {
        return;
    }
    v = p[9];
    if (v == 0xB) {
        goto hit;
    }
    if (v == 0x11) {
        goto hit;
    }
    if (v != 0x10) {
        return;
    }
hit:
    q = &D_8009B574;
    asm volatile("" : "=r"(q) : "0"(q));
    q[0] = 1;
    q[1] = 0xB;
}
