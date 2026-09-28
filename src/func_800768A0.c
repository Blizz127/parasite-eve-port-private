typedef struct {
    short x, y;
    short w, h;
} Rect;

extern short D_80095750;
extern short D_80095752;
extern unsigned int *D_80095850;
extern unsigned int *D_80095854;
extern unsigned int *D_80095858;
extern unsigned int *D_8009585C;
extern unsigned int *D_80095860;

extern void func_800773D0(void);
extern int func_80077404(void);

int func_800768A0(Rect *r0, unsigned int *p0)
{
    register Rect *r asm("$17") = r0;
    register unsigned int *p asm("$18") = p0;
    int size;
    int n;

    func_800773D0();
    r->w = r->w < 0 ? 0 : (D_80095750 < r->w ? D_80095750 : r->w);
    r->h = r->h < 0 ? 0 : (D_80095752 < r->h ? D_80095752 : r->h);
    { int words; { register int t asm("$3") = r->w * r->h + 1; words = t / 2; } size = words >> 4; if (words <= 0) return -1; { int t = size; size = words - (t << 4); n = t; } }
    {
        unsigned int st = *D_80095854;
        unsigned int m;
        asm("" : "=r"(m), "=r"(st) : "0"(0x4000000), "1"(st));
        if (!(st & m)) {
            do {
                if (func_80077404()) {
                    return -1;
                }
            } while (!(*D_80095854 & 0x4000000));
        }
    }
    *D_80095854 = 0x4000000;
    *D_80095850 = 0x1000000;
    *D_80095850 = 0xC0000000;
    *D_80095850 = *(unsigned int *)&r->x;
    *D_80095850 = *(unsigned int *)&r->w;
    {
        unsigned int st = *D_80095854;
        unsigned int m;
        asm("" : "=r"(m), "=r"(st) : "0"(0x8000000), "1"(st));
        if (!(st & m)) {
            do {
                if (func_80077404()) {
                    return -1;
                }
            } while (!(*D_80095854 & 0x8000000));
        }
    }
    {
        int k = -1;
        asm("" : "=r"(k) : "0"(k));
        size--;
        if (size != k) {
            do {
                *p++ = *D_80095850;
            } while (--size != -1);
        }
    }
    if (n) {
        *D_80095854 = 0x4000003;
        *D_80095858 = (unsigned int)p;
        *D_8009585C = (n << 16) | 0x10;
        *D_80095860 = 0x1000200;
    }
    return 0;
}
