/* func_800755F0 — PutDispEnv-shaped display-environment commit.
 * VRAM 0x800755F0 / file 0x65DF0 / size 0x4F8.
 * ERA_CC1_VER=2.8.1 -O2 -G0 -mno-split-addresses + MASPSX_SYMBOL_LOAD_DEST_TEMP=1
 * (profile era_o2_g0_cc1_281_symbol_load_dest_temp). See
 * docs/evidence/func-800755F0/REPORT.md for the lever ladder.
 */
typedef struct { short x, y, w, h; } RECT;
typedef struct { unsigned short x, y, w, h; } VRECT;
typedef struct {
    RECT disp;
    RECT screen;
    unsigned char isinter;
    unsigned char isrgb24;
    unsigned char pad0;
    unsigned char pad1;
} DISPENV;

typedef struct {
    unsigned char level;
    unsigned char dtd;
    unsigned char pad2[0x68];
    DISPENV disp;
} GpuSt;

typedef struct {
    void *f0;
    void *f4;
    void *f8;
    void *fC;
    void (*f10)(unsigned int);
} GpuFn;

extern GpuSt D_8009574E;
extern GpuFn *D_80095744;
extern void (*D_80095748)();
extern char D_80011970;
extern unsigned char D_80095848[];
extern unsigned short D_80095820[][2];
extern int func_80074A28(void);
extern void func_80071A34(void *, void *, int);

#define CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : (v) > (hi) ? (hi) : (v))

DISPENV *func_800755F0(DISPENV *env)
{
    unsigned int mode = 0x08000000;
    int h;
    int c;
    volatile VRECT *r;
    int w, x0, y0, y1, d;
    int sw;
    int i;
    register int x1 asm("$3");

    if (D_8009574E.level >= 2) {
        D_80095748(&D_80011970, env);
    }
    {
        register int yy asm("$3") = (env->disp.y & 0x3FF) << 10;
        D_80095744->f10(yy | 0x05000000 | (env->disp.x & 0x3FF));
    }
    r = (volatile VRECT *)&D_8009574E.disp.disp;
    if (*(int *)&D_8009574E.disp.isinter != *(int *)&env->isinter ||
        (short)r->x != env->disp.x || (short)r->y != env->disp.y ||
        (short)r->w != env->disp.w || (short)r->h != env->disp.h) {
        env->pad0 = func_80074A28();
        if (env->pad0 == 1) {
            mode |= 0x8;
        }
        if (env->isrgb24) {
            mode |= 0x10;
        }
        if (env->isinter) {
            mode |= 0x20;
        }
        if (D_8009574E.dtd) {
            mode |= 0x80;
        }
        w = env->disp.w;
        if (w > 280) {
            if (w <= 352) {
                mode |= 0x1;
            } else if (w <= 400) {
                mode |= 0x40;
            } else if (w <= 560) {
                mode |= 0x2;
            } else {
                mode |= 0x3;
            }
        }
        h = env->disp.h;
        c = env->pad0 ? h < 289 : h < 257;
        if (!c) {
            mode |= 0x24;
        }
        D_80095744->f10(mode);
        env->pad0 = 8;
    }
    r = (volatile VRECT *)&D_8009574E.disp.screen;
    if ((short)r->x != env->screen.x || (short)r->y != env->screen.y ||
        (short)r->w != env->screen.w || (short)r->h != env->screen.h ||
        env->pad0 == 8) {
        env->pad0 = func_80074A28();
        x0 = env->screen.y;
        if (env->pad0 != 0) {
            y0 = x0 + 19;
        } else {
            y0 = x0 + 16;
        }
        if (env->screen.h != 0) {
            y1 = y0 + env->screen.h;
        } else {
            y1 = y0 + 240;
        }
        w = env->disp.w;
        if (w < 281) {
            i = 0;
        } else if (w < 353) {
            i = 1;
        } else if (w < 401) {
            i = 2;
        } else if (w < 561) {
            i = 3;
        } else {
            i = 4;
        }
        d = -D_80095820[env->pad0 * 5 + i][0] + D_80095820[env->pad0 * 5 + i][1];
        x0 = D_80095820[env->pad0 * 5 + i][0] + env->screen.x * D_80095848[i];
        sw = env->screen.w;
        if (sw != 0) {
            d = (d * sw) >> 8;
        }
        x1 = x0 + d;
        asm("" : : "r"(d));
        if (((volatile DISPENV *)env)->pad0 != 0) {
            x0 = CLAMP(x0, 540, 3220);
            x1 = CLAMP(x1, x0 + D_80095848[i] * 4, 3260);
            y0 = CLAMP(y0, 19, 303);
            y1 = CLAMP(y1, y0 + 2, 305);
        } else {
            x0 = CLAMP(x0, 500, 3250);
            x1 = CLAMP(x1, x0 + D_80095848[i] * 4, 3290);
            y0 = CLAMP(y0, 16, 257);
            y1 = CLAMP(y1, y0 + 2, 258);
        }
        D_80095744->f10(((x1 & 0xFFF) << 12) | 0x06000000 | (x0 & 0xFFF));
        {
            register int yy asm("$3") = (y1 & 0x3FF) << 10;
            D_80095744->f10(yy | 0x07000000 | (y0 & 0x3FF));
        }
    }
    func_80071A34(&D_8009574E.disp, env, 20);
    asm("" : : "r"(env));
    return env;
}
