/*
 * func_80077E64 - vram 0x80077E64, size 0x104 (lane4 recorded 0x118, which swallows the next 0x14 bytes). Draft by lane4.
 * era: ERA_CC1_VER=2.8.1  cc1 -O2 -G0 -mno-split-addresses
 *      MASPSX_EXPAND_DIV=1 MASPSX_DIV_NO_REUSE_NOP=1 (maspsx patch 16; 19 words
 *      without it: retail has no nop between the div macro's `mflo $a0` and
 *      the `slti` that reads it).
 */
extern void func_80078FAC(int);
extern void func_80078FB8(int);
void func_80077E64(int a, int b, int h) {
    int d = b - a;
    int c, s, q, t;
    if (d < 100) {
        return;
    }
    c = (-a * b) / d;
    s = (b << 12) / d;
    q = c * 256 / h;
    t = s << 12;
    if (q < -0x8000) q = -0x8000;
    if (q > 0x7FFF) q = 0x7FFF;
    func_80078FAC(q);
    func_80078FB8(t);
}
