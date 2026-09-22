/* NON-MATCHING. 27 word mismatches at -O2 -G0 (vram 0x80072950, size 0x100),
 * all downstream of ONE word: residual class (a), macro-split into a delay
 * slot. Retail emits `lui $a1,%hi(D_800A32D8)` / `jal func_80072A54` /
 * `addiu $a1,$a1,%lo(D_800A32D8)` (delay); maspsx emits both halves of the
 * `la` before the jal and a nop in the slot. Third instance of this class
 * after func_80074BB8 (jalr) and func_80073BBC (jal), and the first where
 * the split symbol is the SECOND argument of a direct jal. */
extern int D_800A32D0;
extern int D_800A32D8;
extern int func_80072A54(int a0, int *a1);

int func_80072950(int *a0, int a1, int a2)
{
    unsigned char *p;
    register unsigned char *end asm("$17");
    register unsigned char *e2 asm("$3");
    register unsigned int n asm("$3");
    register unsigned int t1 asm("$2");
    int h;

    if (*a0 == 0) {
        *a0 = 1;
    }
    n = *(unsigned int *)0x154 / 80;
    p = *(unsigned char **)0x150;
    h = D_800A32D0;
    t1 = n * 4;
    t1 = t1 + n;
    t1 = t1 * 16;
    e2 = p + t1;
    if (p < e2) {
        end = e2;
        do {
            if (*(int *)p != 0) {
                if (func_80072A54(*(int *)p, &D_800A32D8) == 0) {
                    *(int *)(p + 0x34) = h;
                    goto done;
                }
            }
            p += 0x50;
        } while (p < end);
    }
done:
    return (*(int (**)(int *, int, int)) & D_800A32D0)(a0, a1, a2);
}
