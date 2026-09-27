/*
 * func_800832B4 - vram 0x800832B4, size 0x234. libpad SIO byte exchange with timer timeout (PadSioRW2).
 * era: ERA_CC1_VER=2.8.1 -O2 -G0 -mno-split-addresses + MASPSX_LOAD_DELAY_NOP_BEFORE_LABEL=1
 * (patch 14; the first poll loop label). Also exact on cc1 2.7.2 + patch 14 + patch 3.
 * Levers: sibling of func_800830DC (block-local int w for the sra test); mode pinned $17;
 * sh-mode arm first; natural loop-invariant hoist of the SIO timer addresses; the buffer index is
 * a fresh volatile-cast read pinned to $3 behind an empty register barrier (retail reloads it).
 */
extern volatile unsigned char *D_8009B788;
extern volatile unsigned int *D_8009B784;
extern unsigned int D_800BD02C;
extern unsigned int D_800A76D0;
extern int D_8009B768;
extern void func_80084FC4(int);
extern int func_80084FE4(void);

int func_800832B4(unsigned char *a0, int a1)
{
    register int mode asm("$17");
    int r;
    volatile unsigned char *b;

    mode = 0x88;
    {
        int w = **(unsigned char **)(a0 + 0x3C);

        if ((w >> 4) == 8 && a0[0x44] >= 9) {
            mode = 0x22;
        }
    }
    {
        volatile unsigned char *c = D_8009B788;

        while (!(*(volatile unsigned short *)(c + 4) & 2)) {
        }
    }
    func_80084FC4(0x190);
    b = D_8009B788;
    r = *b;
    if (a0[0x44] != 0 || (r >> 4) != 8) {
        *(volatile short *)(b + 0xE) = mode;
    } else {
        *(volatile short *)(b + 0xE) = 0x22;
    }
    if (!(*D_8009B784 & 0x80)) {
        do {
            unsigned int t = *(volatile unsigned short *)0x1F801120;

            if (t < D_800A76D0) {
                if (*(volatile unsigned short *)0x1F801128 != 0) {
                    t = *(volatile unsigned short *)0x1F801128 + t;
                } else {
                    t += 0x10000;
                }
            }
            if (*(volatile unsigned short *)0x1F801124 & 0x200) {
                if (t - D_800A76D0 >= D_800BD02C) {
                    return -2;
                }
            }
            if ((t - D_800A76D0) >> 3 >= D_800BD02C) {
                return -2;
            }
        } while (!(*D_8009B784 & 0x80));
    }
    if (a0[0xE8] != 8 && D_8009B768 == 2) {
        func_80084FC4(0x3C);
        while (func_80084FE4() == 0) {
        }
    }
    *D_8009B788 = a1;
    a0[0x45]++;
    if (a0[0x44] != 0xFF) {
        register int i asm("$3") = ((volatile unsigned char *)a0)[0x44];

        __asm__("" : "=r"(i) : "0"(i));
        (*(unsigned char **)(a0 + 0x3C))[i] = r;
    }
    a0[0x44]++;
    return r;
}
