/* VRAM 0x80074E28 / file 0x65628 / size 0x11C.
 *
 * GPU debug rectangle check: at debug level 1 report an out-of-bounds RECT,
 * at level 2 report every RECT, through the D_80095748 printf hook.
 * ERA_CC1_VER=2.8.1 -O2 -G0 -mno-split-addresses + maspsx patch 8
 * (MASPSX_FILL_CALL_DELAY_SLOT=1; the `la` of each format string splits
 * into the jalr slot). Levers: pointer local p = &D_8009574E and r-first
 * compares (41 -> 26 -> 9 words); `register int h asm("$3")` loaded last
 * reproduces retail's lh $v1 after x/y/w and lets cc1 fill the final jalr
 * slot with `sw $v1,0x10($sp)` (9 -> 0). Without patch 8: 26 words.
 */
typedef struct { short x, y, w, h; } RECT;
typedef struct { unsigned char level; unsigned char pad; short w; short h; } GpuDbg;
extern GpuDbg D_8009574E;
extern void (*D_80095748)();
extern char D_80011898;
extern char D_800118B8;
extern char D_800118A4;
void func_80074E28(char *name, RECT *r) {
    GpuDbg *p = &D_8009574E;
    char *fmt;
    switch (p->level) {
    case 1:
        if (r->w > p->w || r->w + r->x > p->w ||
            r->y > p->h || r->y + r->h > p->h ||
            r->w <= 0 || r->x < 0 || r->y < 0 || r->h <= 0) {
            fmt = &D_80011898;
            break;
        }
        return;
    case 2:
        fmt = &D_800118B8;
        break;
    default:
        return;
    }
    D_80095748(fmt, name);
    { register int h asm("$3"); int x = r->x, y = r->y, w = r->w; h = r->h; D_80095748(&D_800118A4, x, y, w, h); }
}
