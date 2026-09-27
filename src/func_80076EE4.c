extern volatile unsigned int *D_80095860;
extern volatile unsigned int *D_80095854;
extern volatile int D_80095874;
extern volatile int D_80095878;
extern int D_80095880;
extern int D_80095758;
extern int D_80095754;
extern unsigned int D_800BD030[];
extern unsigned int D_800BD034[];
extern unsigned int D_800BD038[];
extern int func_80073E10(int);
extern void func_80073CF4(int, int);

int func_80076EE4(void) {
    int saved;
    int prod;
    unsigned int ready;
    unsigned int busy;
    register void (*cb)() asm("$4");
    register int *mp asm("$3");
    register int *mq asm("$2");

    if (*D_80095860 & 0x01000000) {
        return 1;
    }
    saved = func_80073E10(0);
    prod = D_80095874;
    D_80095880 = saved;
    if (prod != D_80095878 && (*D_80095860 & 0x01000000) == 0) {
        ready = 0x04000000;
        busy = 0x01000000;
        do {
            if (((D_80095878 + 1) & 0x3F) == D_80095874 && D_80095758 == 0) {
                func_80073CF4(2, 0);
            }
            {
                volatile unsigned int *st = D_80095854;
                register int idx asm("$5");
                register int cidx asm("$5");
                int triple;
                int arg;
                int aux;
                void (*fn)(int, int);

                if ((*st & ready) == 0) {
                    unsigned int m = 0x04000000;
                    do {
                    } while ((*st & m) == 0);
                }
                idx = D_80095878;
                arg = D_800BD034[D_80095878 * 24];
                triple = (idx << 1) + idx;
                asm volatile("" : "=r"(arg) : "0"(arg));
                cidx = D_80095878;
                aux = D_800BD038[cidx * 24];
                fn = *(void (**)(int, int))((char *)&D_800BD030 + (triple << 5));
                fn(arg, aux);
            }
            D_80095878 = (D_80095878 + 1) & 0x3F;
        } while (D_80095874 != D_80095878 && (*D_80095860 & busy) == 0);
    }
    func_80073E10(D_80095880);
    if (D_80095874 == D_80095878 && (*D_80095860 & 0x01000000) == 0) {
        mp = &D_80095754;
        if (*mp != 0) {
            cb = (void (*)())mp[1];
            if (cb != 0) {
                mq = mp - 2;
                asm volatile("" : "=r"(mq) : "0"(mq));
                *(volatile int *)(mq + 2) = 0;
                cb();
            }
        }
    }
    return (D_80095874 - D_80095878) & 0x3F;
}
