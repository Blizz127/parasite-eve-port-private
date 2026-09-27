/*
 * func_80076434 - vram 0x80076434, size 0x230. libgpu ClearImage (Gpu_DmaVramTransfer).
 * era: ERA_CC1_VER=2.8.1 -O2 -G0 -mno-split-addresses + MASPSX_FILL_CALL_DELAY_SLOT=1 (patch 8).
 * Lane4 draft (5w) + an empty register barrier on the return value: `int ret = 0;` barriered keeps
 * `move v0,zero` right after the final jal (before the lw ra/s* restores) where 2.8.1 sched2 would
 * otherwise sink it below them. No memory clobber.
 */
typedef struct { short x, y, w, h; } RECT;
extern short D_80095750;
extern short D_80095752;
extern unsigned int D_800A3300[];
extern unsigned int D_800A3328[];
extern unsigned int *D_80095854;
extern unsigned int func_80076BE0(int);
extern void func_80076B98(unsigned int *);

int func_80076434(RECT *r, unsigned int color) {
    r->w = (r->w < 0) ? 0 : ((r->w > D_80095750 - 1) ? D_80095750 - 1 : r->w);
    r->h = (r->h < 0) ? 0 : ((r->h > D_80095752 - 1) ? D_80095752 - 1 : r->h);
    if ((r->x & 0x3F) || (r->w & 0x3F)) {
        D_800A3300[0] = ((unsigned int)D_800A3328 & 0xFFFFFF) | 0x8000000;
        D_800A3300[1] = 0xE3000000;
        D_800A3300[2] = 0xE4FFFFFF;
        D_800A3300[3] = 0xE5000000;
        D_800A3300[4] = 0xE6000000;
        D_800A3300[5] = 0xE1000000 | (*D_80095854 & 0x7FF) | ((color >> 31) << 10);
        D_800A3300[6] = (color & 0xFFFFFF) | 0x60000000;
        D_800A3300[7] = ((unsigned int *)r)[0];
        D_800A3300[8] = ((unsigned int *)r)[1];
        D_800A3328[0] = 0x3FFFFFF;
        D_800A3328[1] = func_80076BE0(3) | 0xE3000000;
        D_800A3328[2] = func_80076BE0(4) | 0xE4000000;
        D_800A3328[3] = func_80076BE0(5) | 0xE5000000;
    } else {
        D_800A3300[0] = 0x5FFFFFF;
        D_800A3300[1] = 0xE6000000;
        D_800A3300[2] = 0xE1000000 | (*D_80095854 & 0x7FF) | ((color >> 31) << 10);
        D_800A3300[3] = (color & 0xFFFFFF) | 0x2000000;
        D_800A3300[4] = ((unsigned int *)r)[0];
        D_800A3300[5] = ((unsigned int *)r)[1];
    }
    func_80076B98(D_800A3300);
    {
        int ret = 0;

        __asm__("" : "=r"(ret) : "0"(ret));
        return ret;
    }
}
