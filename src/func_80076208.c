/*
 * func_80076208 — clamp (x, y) to [0, D_80095750-1] x [0, D_80095752-1] and pack
 * GP0 0xE4 (drawing-area corner): 0xE4000000 | (y & 0x3FF) << 10 | (x & 0x3FF).
 *
 * ROM: ERA_CC1_VER=2.8.1 -O2 -G0 -mno-split-addresses (band compiler). Under
 * 2.7.2 the short-typed ?: results get a 16-byte stack temp; 2.8.1 -O1 is 27+
 * words off.
 */
extern short D_80095750;
extern short D_80095752;

unsigned int func_80076208(short x, short y) {
    x = (x < 0) ? 0 : ((x > D_80095750 - 1) ? D_80095750 - 1 : x);
    y = (y < 0) ? 0 : ((y > D_80095752 - 1) ? D_80095752 - 1 : y);
    return 0xE4000000 | ((y & 0x3FF) << 10) | (x & 0x3FF);
}
