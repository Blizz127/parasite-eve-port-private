/*
 * func_80075CE8 — build a DR_ENV packet from a DRAWENV (SetDrawEnv-style):
 * draw-area corners (func_80076170/80076208), offset (func_800762A0), texpage
 * (func_80076150), texture window (func_800762BC), 0xE6000000 mask word, and
 * when `isbg` is set a 0x60 fill rectangle clamped to the display extent
 * (D_80095750/52). The packet length goes in byte 3.
 *
 * ROM: ERA_CC1_VER=2.8.1 -O2 -G0 -mno-split-addresses. Source from the earlier
 * session's draft; the `register ... asm("$8")`/`asm("$2")` pins are load-bearing.
 */
struct ENV {
    short x;
    short y;
    unsigned short w;
    unsigned short h;
    short ox;
    short oy;
    unsigned char c0C[8];
    unsigned short f14;
    unsigned char b16;
    unsigned char b17;
    unsigned char b18;
    unsigned char b19;
    unsigned char b1A;
    unsigned char b1B;
};

extern short D_80095750;
extern short D_80095752;
extern int func_80076170(int a0, int a1);
extern int func_80076208(int a0, int a1);
extern int func_800762A0(int a0, int a1);
extern int func_80076150(int a0, int a1, int a2);
extern int func_800762BC(unsigned char *a0);

void func_80075CE8(void *dd, struct ENV *env)
{
    unsigned int *dr = (unsigned int *)dd;
    struct { short x; short y; unsigned short w; unsigned short h; } r;
    register int t asm("$8");
    unsigned short v;
    register int lenv asm("$2");
    unsigned int tw;
    unsigned int tx;
    unsigned int ty;
    unsigned int th;
    int aa;
    int bb;

    dr[1] = func_80076170(env->x, env->y);
    tw = env->w;
    tx = (unsigned short)env->x;
    ty = (unsigned short)env->y;
    aa = (short)(tw + tx - 1);
    th = env->h;
    bb = (short)(ty + th - 1);
    dr[2] = func_80076208(aa, bb);
    dr[3] = func_800762A0(env->ox, env->oy);
    dr[4] = func_80076150(env->b17, env->b16, env->f14);
    dr[5] = func_800762BC(env->c0C);
    dr[6] = 0xE6000000;
    t = 7;
    if (env->b18 != 0) {
        r.x = env->x;
        r.y = env->y;
        r.w = env->w;
        r.h = env->h;
        if ((short)r.w < 0) {
            v = 0;
        } else if ((short)D_80095750 - 1 < (short)r.w) {
            v = (unsigned short)D_80095750 - 1;
        } else {
            v = r.w;
        }
        r.w = v;
        if ((short)r.h >= 0) {
            if ((short)D_80095752 - 1 < (short)r.h) {
                v = (unsigned short)D_80095752 - 1;
            } else {
                v = r.h;
            }
        } else {
            v = 0;
        }
        r.h = v;
        r.x = r.x - env->ox;
        r.y = r.y - env->oy;
        dr[t] = 0x60000000 | (env->b1B << 16) | (env->b1A << 8) | env->b19;
        t++;
        dr[t] = ((int *)&r)[0];
        t++;
        dr[t] = ((int *)&r)[1];
        t++;
    }
    lenv = t - 1;
    ((unsigned char *)dr)[3] = lenv;
}
