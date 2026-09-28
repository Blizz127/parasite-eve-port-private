typedef struct {
    unsigned short active;
    unsigned short pad;
    void *cb[11];
    unsigned short mask;
} IntrTab;

extern void *D_800945E8[];
extern volatile unsigned short *D_80095674;
extern void func_80073C74(int a0);
extern void func_80073C84(int a0, int a1);

void *func_800740D0(int idx_, void *cb_)
{
    register int idx asm("$17") = idx_;
    register void *cb asm("$18") = cb_;
    unsigned int m;
    register void **tab asm("$5") = D_800945E8;
    void **slot = &tab[idx];
    void *old;
    register IntrTab *t asm("$6");

    __asm__("" : "=r"(slot) : "0"(slot));
    old = *slot;
    if (cb == old) {
        goto out;
    }
    t = (IntrTab *)((char *)tab - 4);
    if (t->active == 0) {
        goto out;
    }
    {
        unsigned int r = *D_80095674;
        *D_80095674 = 0;
        m = r & 0xFFFF;
    }
    if (cb != 0) {
        *slot = cb;
        m |= 1 << idx;
        t->mask |= 1 << idx;
    } else {
        __asm__("" : "=r"(cb) : "0"(cb));
        *slot = 0;
        {
            register unsigned int nb asm("$2") = ~(1 << idx);
            m &= nb;
            *(unsigned short *)((char *)tab + 0x2C) &= nb;
        }
    }
    if (idx == 0) {
        func_80073C74(cb == 0);
        func_80073C84(3, cb == 0);
    }
    if (idx == 4) {
        func_80073C84(0, cb == 0);
    }
    if (idx == 5) {
        func_80073C84(1, cb == 0);
    }
    if (idx == 6) {
        func_80073C84(2, cb == 0);
    }
    *D_80095674 = m;
out:
    return old;
}
