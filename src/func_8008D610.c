extern int func_80085BB4(unsigned int a0);
extern void func_8007D778();
extern void func_8008D7B0(int a0);
extern int D_8009B46C[];
extern int D_8009B424;
extern int D_8009B418;
extern volatile int D_8009B434;
extern int D_8009B384;
extern unsigned char D_8009C4C0[];

int func_8008D610(int mode)
{
    volatile int saved;
    int was;
    int *slot;
    int size;
    int addr;
    int cont;
    int old;
    int chunk;
    unsigned int big;

    saved = 0;
    was = 0;
    if ((unsigned int)mode >= 10) {
        return -1;
    }
    {
        register int *base asm("$3");
        register int off asm("$2");

        base = D_8009B46C;
        off = mode << 2;
        slot = (int *)((unsigned int)base + (unsigned int)off);
    }
    if (func_80085BB4((unsigned int)*slot) != 0) {
        return -1;
    }
    big = 0x10000;
    if (mode == 0) {
        register int sh asm("$2");
        register int k asm("$3");

        sh = D_8009B424;
        k = 0x10;
        size = k << sh;
        k = 0xFFF0;
        addr = k << sh;
    } else {
        register int ent asm("$4");
        register int sh asm("$3");

        ent = *slot;
        sh = D_8009B424;
        size = (int)(big - (unsigned int)ent) << sh;
        addr = ent << sh;
    }
    old = D_8009B418;
    if (old == 1) {
        D_8009B418 = 0;
        was = 1;
    }
    cont = 1;
    if (D_8009B434 != 0) {
        saved = D_8009B434;
        D_8009B434 = 0;
    }
    do {
        chunk = 0x400;
        if ((unsigned int)size < 0x401) {
            chunk = size;
            cont = 0;
        }
        func_8007D778(2, addr);
        func_8007D778(1);
        func_8007D778(3, D_8009C4C0, chunk);
        {
            register int arg asm("$4");

            arg = D_8009B384;
            asm volatile("" : "=r"(arg) : "0"(arg));
            size = size - 0x400;
            addr = addr + 0x400;
            func_8008D7B0(arg);
        }
    } while (cont != 0);
    if (was != 0) {
        D_8009B418 = old;
    }
    if (saved != 0) {
        D_8009B434 = saved;
    }
    return 0;
}
