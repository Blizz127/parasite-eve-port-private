typedef struct { short x, y, w, h; } RECT;
typedef struct { unsigned int tag; unsigned int cmd; unsigned int xy; unsigned int col; unsigned int wh; } PKT;
extern char D_800118EC;
extern void func_80074E28(char *name, RECT *r);
extern int func_80073A44(int mode);
extern volatile unsigned int *D_80095860;
extern int D_80095888;
extern int D_8009588C;
extern volatile unsigned int *D_80095854;
extern int func_80077404(void);
extern void func_80073CF4(int slot, void (*fn)());
extern void func_80077A00(void);
extern unsigned int *D_80095744;
extern PKT D_800957E4;

int func_800777C0(RECT *r, int a1, int a2)
{
    func_80074E28(&D_800118EC, r);
    D_80095888 = func_80073A44(-1) + 240;
    D_8009588C = 0;
    while ((*D_80095860 & 0x01000000) != 0 || (*D_80095854 & 0x04000000) == 0) {
        if (func_80077404() != 0) {
            return -1;
        }
    }
    func_80073CF4(2, func_80077A00);
    if (r->w == 0 || r->h == 0) {
        return -1;
    }
    D_800957E4.xy = *(unsigned int *)&r->x;
    D_800957E4.col = (a2 << 16) | (a1 & 0xFFFF);
    D_800957E4.wh = *(unsigned int *)&r->w;
    ((void (*)(PKT *))D_80095744[6])(&D_800957E4);
    return 0;
}
