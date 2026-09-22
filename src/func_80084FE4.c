extern unsigned int D_800A76D0;
extern unsigned int D_800BD02C;

int func_80084FE4(void)
{
    register unsigned short raw asm("$3");
    register unsigned int t asm("$4");
    register unsigned int d asm("$2");
    unsigned int base;
    unsigned int lim;

    raw = *(volatile unsigned short *)0x1F801120;
    asm ("" : "=r" (raw) : "0" (raw));
    t = raw;
    if (t < D_800A76D0) {
        if (*(volatile unsigned short *)0x1F801128 != 0) {
            t = t + *(volatile unsigned short *)0x1F801128;
        } else {
            t = t + 0x10000;
        }
    }
    if ((*(volatile unsigned short *)0x1F801124 & 0x200) == 0) {
        base = D_800A76D0;
        lim = D_800BD02C;
        d = (t - base) >> 3;
    } else {
        base = D_800A76D0;
        lim = D_800BD02C;
        d = t - base;
    }
    return (d < lim) ^ 1;
}
