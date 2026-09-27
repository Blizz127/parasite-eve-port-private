/*
 * func_800762BC — texture-window packer (GP0 0xE2): from a RECT, returns
 * 0xE2000000 | (y>>3)<<15 | (x>>3)<<10 | ((-h&0xFF)>>3)<<5 | ((-w&0xFF)>>3),
 * or 0 for a null RECT. The four fields are staged in a 4-word stack array.
 *
 * ROM: era gcc-2.7.2-psx -O2 -G0 + MASPSX_FILL_EPILOGUE_DELAY_SLOT=1 (patch 3;
 * without it the `addiu $sp` / `jr` pair differs by 2 words). The `if (tw)`
 * body-first spelling is load-bearing (early `return 0` inverts the branch).
 */
typedef struct { short x, y, w, h; } RECT;
unsigned int func_800762BC(RECT *tw) {
    int code[4];
    if (tw) {
        code[0] = (tw->x & 0xFF) >> 3;
        code[2] = ((-tw->w) & 0xFF) >> 3;
        code[1] = (tw->y & 0xFF) >> 3;
        code[3] = ((-tw->h) & 0xFF) >> 3;
        return 0xE2000000 | (code[1] << 15) | (code[0] << 10) | (code[3] << 5) | code[2];
    }
    return 0;
}
