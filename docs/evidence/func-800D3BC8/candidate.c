/* func_800D3BC8 — EXE, VRAM 0x800D3BC8, file offset 0xC43C8, 0x39C bytes.
 * Camera-facing textured sprite (one POLY_FT4).  C CANDIDATE, not a YAML
 * leaf: era -O2 -G0 links 24 words off, all in the prologue (callee-save
 * batching vs the first stack-argument load); body, epilogue and size match.
 * See REPORT.md.  Native port: pc_port/game/boot/func_800D3BC8_port.c. */
#define gte_SetRotMatrix(r0) __asm__ volatile ( \
    "lw $12,0(%0)\n\tlw $13,4(%0)\n\tctc2 $12,$0\n\tctc2 $13,$1\n\t" \
    "lw $12,8(%0)\n\tlw $13,12(%0)\n\tlw $14,16(%0)\n\t" \
    "ctc2 $12,$2\n\tctc2 $13,$3\n\tctc2 $14,$4" : : "r"(r0) : "$12", "$13", "$14")
#define gte_SetTransMatrix(r0) __asm__ volatile ( \
    "lw $12,20(%0)\n\tlw $13,24(%0)\n\tctc2 $12,$5\n\tlw $14,28(%0)\n\t" \
    "ctc2 $13,$6\n\tctc2 $14,$7" : : "r"(r0) : "$12", "$13", "$14")
#define gte_ldv0(r0) __asm__ volatile ("lwc2 $0,0(%0)\n\tlwc2 $1,4(%0)" : : "r"(r0))
#define gte_rtps() __asm__ volatile ("nop\n\tnop\n\t.word 0x4A180001")
#define gte_stsxy(r0) __asm__ volatile ("swc2 $14,0(%0)" : : "r"(r0) : "memory")
#define gte_stszotz(r0) __asm__ volatile ( \
    "mfc2 $12,$19\n\tnop\n\tsra $12,$12,2\n\tsw $12,0(%0)" : : "r"(r0) : "$12", "memory")

typedef struct { short vx, vy, vz, pad; } SVECTOR;
typedef struct { short vx, vy; } DVECTOR;
typedef struct { short m[3][3]; int t[3]; } MATRIX;
typedef struct {
    unsigned int tag;
    unsigned char r0, g0, b0, code;
    short x0, y0;
    unsigned char u0, v0; unsigned short clut;
    short x1, y1;
    unsigned char u1, v1; unsigned short tpage;
    short x2, y2;
    unsigned char u2, v2; unsigned short pad1;
    short x3, y3;
    unsigned char u3, v3; unsigned short pad2;
} POLY_FT4;
typedef struct { MATRIX *value; unsigned char reserved[8]; } RenderMatrixSlot;
typedef struct { char *ordering[2]; char *other_buffers[6]; char *packets[2]; } RenderBufferPrefix;
extern RenderBufferPrefix D_800B0E38;
extern RenderMatrixSlot D_800BCFA4;
extern int D_8009CDD8;
extern int D_8009CDDC;
typedef struct {
    unsigned short p00, p02, palette, p06, tpage, p0A;
    unsigned short depth;
    unsigned short extent_x, extent_y;
} RenderEffectParameters;
extern RenderEffectParameters D_800F3368;
extern int func_80077A64();
extern void func_80077AC4();
extern void func_800783E4(void *first, void *second, int first_scale, int second_scale, void *out);

void func_800D3BC8(SVECTOR *pos, int sx, int sy, int cell, int clut, int abr, int bright,
                   unsigned char *color, int lerp)
{
    DVECTOR center;
    DVECTOR xy;
    int otz;
    POLY_FT4 *prim;
    MATRIX **slot;
    int u, v, w, h;
    int r, g, b;

    center.vx = 160;
    center.vy = 120;
    prim = (POLY_FT4 *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(POLY_FT4);
    if (color == 0) {
        r = g = b = bright;
    } else {
        r = bright * color[0] / 128;
        g = bright * color[1] / 128;
        b = bright * color[2] / 128;
    }
    prim->r0 = r;
    prim->g0 = g;
    prim->b0 = b;
    if (D_800F3368.p06 != 0) {
        u = cell & 0xF;
        v = 0;
        if (u >= 8) {
            u -= 8;
            v = 32;
        }
        u <<= 4;
        v += (cell / 16) << 4;
    } else {
        u = (cell & 0xF) << 4;
        v = (cell / 16) << 4;
    }
    slot = &D_800BCFA4.value;
    __asm__ volatile ("" : "=r"(slot) : "0"(slot));
    gte_SetRotMatrix(*slot);
    gte_SetTransMatrix(*slot);
    gte_ldv0(pos);
    gte_rtps();
    ((unsigned char *)prim)[3] = 9;
    prim->code = 0x2C;
    if (abr == 0xFF) {
        prim->code = 0x2C;
        prim->tpage = D_800F3368.tpage;
    } else {
        prim->code = 0x2E;
        prim->tpage = D_800F3368.tpage | func_80077A64(0, abr, 0, 0);
    }
    prim->clut = clut;
    w = D_800F3368.extent_x;
    h = D_800F3368.extent_y;
    prim->u0 = prim->u2 = u;
    prim->v1 = prim->v0 = v;
    prim->u1 = prim->u3 = u + w - 1;
    prim->v2 = prim->v3 = v + h - 1;
    w = w * sx / 8192;
    h = h * sy / 8192;
    gte_stszotz(&otz);
    otz -= D_800F3368.depth;
    if (lerp != 0) {
        otz = 14;
    }
    if ((unsigned int)otz < 0x1000) {
        gte_stsxy(&xy);
        lerp *= 2;
        func_800783E4(&xy, &center, 0x1000 - lerp, lerp, &xy);
        prim->x0 = prim->x2 = xy.vx - w;
        prim->y0 = prim->y1 = xy.vy - h;
        prim->x1 = prim->x3 = xy.vx + w;
        prim->y2 = prim->y3 = xy.vy + h;
        func_80077AC4(D_800B0E38.ordering[D_8009CDDC] + (otz << 2), prim);
    }
}
