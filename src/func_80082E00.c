/*
 * func_80082E00 - SIO pad port select + pending-slot drain.  vram 0x80082E00,
 * size 0x214.  era cc1 2.7.2 -O2 -G0 + MASPSX_FILL_BRANCH_MACRO_SPLIT=1 +
 * MASPSX_FILL_EPILOGUE_DELAY_SLOT=1.
 *
 * Levers: the drain loop uses its own pointer `lq` built from a `$3`-pinned
 * scaled index (`(D_8009B764 << 2) + tbl`, index first) so retail's
 * `sll $v1` / `addu $v1,$v1,$s1` survives; the loop-exit re-read is spelled
 * `*(D_8009B764 + tbl)`.  The post-loop clear loads the handler into `f`
 * before the `*q = -1` store so reorg puts the store in the `jalr` slot.
 */
extern volatile unsigned char *D_8009B788;
extern volatile int *D_8009B784;
extern int D_8009B764;
extern int D_8009B77C[];
extern void (*D_8009B744)(int a0);
extern void (*D_8009B748)(int a0);
extern void func_80084FC4(int a0);
extern void func_80083578(void);

int func_80082E00(unsigned char *a0)
{
    unsigned char *sio;
    int n;
    int *q;
    int *tbl;
    int mode;
    int idx;
    int *lq;
    void (*f)(int);
    register int ctl asm("$3");
    register unsigned char *sio2 asm("$4");

    sio = (unsigned char *)D_8009B788;
    *(volatile short *)(sio + 0xA) = 0x40;
    *(volatile short *)(sio + 0xA) = 0;
    *(volatile short *)(sio + 8) = 0xD;
    *(volatile short *)(sio + 0xE) = 0x88;
    mode = 0x91;
    if (a0[0xE8] == 8) {
        mode = 0x50;
    }
    func_80084FC4(mode);
    idx = D_8009B764;
    sio2 = (unsigned char *)D_8009B788;
    if (idx != 0) {
        ctl = 0x3003;
    } else {
        ctl = 0x1003;
    }
    *(volatile short *)(sio2 + 0xA) = ctl;
    q = &D_8009B77C[idx];
    n = *q;
    if (n >= 0) {
        if (n > 0) {
            tbl = D_8009B77C;
            for (;;) {
                {
                    register int k asm("$3") = D_8009B764 << 2;
                    lq = (int *)(k + (int)tbl);
                }
                n = *lq - 1;
                *lq = n;
                D_8009B744(*(int *)(a0 + 0xC) + n * 240);
                if (*(D_8009B764 + tbl) <= 0) {
                    break;
                }
            }
        }
        q = &D_8009B77C[D_8009B764];
        if (*q == 0) {
            f = D_8009B744;
            *q = -1;
            f((int)a0);
            D_8009B748((int)a0);
        }
    }
    sio = (unsigned char *)D_8009B788;
    if ((*(volatile unsigned short *)(sio + 4) & 0x200) != 0) {
        *(volatile unsigned short *)(sio + 0xA) =
            *(volatile unsigned short *)(sio + 0xA) | 0x10;
        if ((*(volatile unsigned short *)(sio + 4) & 0x200) != 0) {
            *(volatile unsigned char *)sio = 1;
            func_80083578();
            D_8009B788[0];
            return 0;
        }
        *D_8009B784 = -0x81;
    }
    if (a0[0x50] != 0) {
        if (a0[0x36] != 0) {
            return 0;
        }
    }
    *(unsigned char *)*(int *)(a0 + 0x3C) = 0;
    return 1;
}
