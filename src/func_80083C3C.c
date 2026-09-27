/*
 * func_80083C3C - per-slot button remap: for each of a0[0xE9] slots count the
 * six map bytes that name the slot, then mark the excess ones 0xFF.
 * vram 0x80083C3C, size 0xC8.  era cc1 2.7.2 -O2 -G0 (default).
 *
 * Levers: the outer loop is a guarded do/while (a plain `for` makes cc1
 * seed the `k * 5` giv from k's register instead of a literal 0), and the
 * row index is written `k * 5 + 2` so loop strength reduction owns the
 * stride-5 offset.
 */
int func_80083C3C(unsigned char *a0)
{
    int k;
    int j;
    int cnt;
    int lim;
    unsigned char *p;

    k = 0;
    if (a0[0xE9] != 0) {
        do {
        p = *(unsigned char **)(a0 + 0x20);
        cnt = 0;
        for (j = 5; j >= 0; j--) {
            if (*p++ == k) {
                cnt++;
            }
        }
        lim = (*(unsigned char **)(a0 + 4))[k * 5 + 2];
        p = *(unsigned char **)(a0 + 0x20);
        if (lim == 0) {
            lim = 1;
        }
        for (j = 0; j < 6; j++) {
            if (*p++ == k) {
                if (cnt < lim) {
                    a0[0x5D + j] = 0xFF;
                    cnt--;
                } else {
                    a0[0x5D + j] = k;
                }
            }
        }
        } while (++k < a0[0xE9]);
    }
    a0[0x46] = 0xFE;
    return 0;
}
