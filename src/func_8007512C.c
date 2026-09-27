/*
 * func_8007512C — log via func_80074E28(&D_800118EC, r); reject an empty RECT
 * (w == 0 or h == 0 -> -1); else fill the D_800957E4 packet (xy, packed
 * a2<<16 | a1&0xFFFF, wh) and hand it to D_80095744->f(0x8)(v[6], pkt, 0x14, 0).
 *
 * ROM: ERA_CC1_VER=2.8.1 -O2 -G0 -mno-split-addresses. Load-bearing: the
 * packet is a struct global (retail anchors the base on the +8 `xy` field,
 * D_800957EC, and passes base-8) and the stores are written xy, col, wh —
 * col-first anchors on +0xC instead (14 words).
 */
typedef struct { short x, y, w, h; } RECT;
typedef struct { unsigned int tag; unsigned int cmd; unsigned int xy; unsigned int col; unsigned int wh; } PKT;
extern unsigned int *D_80095744;
extern PKT D_800957E4;
extern char D_800118EC;
extern void func_80074E28(char *a0, RECT *a1);

int func_8007512C(RECT *r, int a1, int a2) {
    func_80074E28(&D_800118EC, r);
    if (r->w == 0 || r->h == 0) {
        return -1;
    }
    D_800957E4.xy = *(unsigned int *)&r->x;
    D_800957E4.col = (a2 << 16) | (a1 & 0xFFFF);
    D_800957E4.wh = *(unsigned int *)&r->w;
    return (*(int (**)(unsigned int, PKT *, int, int))(D_80095744 + 2))(D_80095744[6], &D_800957E4, 0x14, 0);
}
