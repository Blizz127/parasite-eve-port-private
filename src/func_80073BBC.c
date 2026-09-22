/* NON-MATCHING. 21 word mismatches at -O2 -G0 (vram 0x80073BBC, size 0x98).
 * Matches from the second word on; residual class (a): macro-split into a
 * delay slot -- retail puts the %lo half of `la $a0,D_800116FC` in the
 * jal delay slot. `extern volatile int D_800956AC` is load-bearing (the
 * volatile-vs-volatile ordering keeps the sll/sw before the global load).
 * Unchanged by ERA_ASPSX_VER 2.21/2.30/2.56/2.70. */
extern volatile int D_800956AC;
extern char D_800116FC;
extern void func_80073C5C(char *a0);
extern void func_80073C74(int a0);
extern void func_80073C84(int a0, int a1);

void func_80073BBC(int a0, int a1)
{
    volatile int n;

    n = a1 << 15;
    if (D_800956AC < a0) {
        do {
            n = n - 1;
            if (n == -1) {
                func_80073C5C(&D_800116FC);
                func_80073C74(0);
                func_80073C84(3, 0);
                return;
            }
        } while (D_800956AC < a0);
    }
}
