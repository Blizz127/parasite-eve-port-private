/*
 * func_800830DC - vram 0x800830DC, size 0x1D8. libpad SIO byte exchange.
 * era: cc1 2.7.2 -O2 -G0 + MASPSX_FILL_EPILOGUE_DELAY_SLOT=1 (patch 3; 2 words without).
 * Levers: guarded poll loops, D_8009B784 read into q before the volatile sh,
 * single shared `return r` (goto out), block-local int w for the sra test.
 */
extern volatile unsigned char *D_8009B788;
extern volatile unsigned int *D_8009B784;
extern int D_800BD02C;
extern int D_800A76D0;
extern int func_80084FE4(void);

int func_800830DC(unsigned char *a0, int a1)
{
    int r;
    int mode;
    int v;
    int t;
    volatile unsigned char *b;
    volatile unsigned int *q;

    if (a1 < 0) {
        r = *D_8009B788;
        a0[0x44] = 0xFF;
        a0[0x45] = 1;
        **(unsigned char **)(a0 + 0x40) = ~a1;
        b = D_8009B788;
        if (!(*(volatile unsigned short *)(b + 4) & 1)) {
            do {
            } while (!(*(volatile unsigned short *)(b + 4) & 1));
        }
        do {
        } while (func_80084FE4() == 0);
        *D_8009B788 = ~a1;
        goto out;
    }
    mode = 0x88;
    {
        int w = **(unsigned char **)(a0 + 0x3C);

        if ((w >> 4) == 8 && a0[0x44] >= 9) {
            mode = 0x22;
        }
    }
    t = *(volatile unsigned short *)0x1F801120;
    v = *(volatile unsigned short *)(D_8009B788 + 4);
    D_800BD02C = 0x1AE;
    D_800A76D0 = t;
    if (!(v & 2)) {
        b = D_8009B788;
        do {
        } while (!(*(volatile unsigned short *)(b + 4) & 2));
    }
    q = D_8009B784;
    r = *D_8009B788;
    *(volatile short *)(D_8009B788 + 0xE) = mode;
    if (!(*q & 0x80)) {
        do {
            if (func_80084FE4() != 0) {
                return -0x14;
            }
        } while (!(*D_8009B784 & 0x80));
    }
    *D_8009B788 = a1;
    a0[0x45]++;
    (*(unsigned char **)(a0 + 0x3C))[a0[0x44]] = r;
    a0[0x44]++;
out:
    return r;
}
