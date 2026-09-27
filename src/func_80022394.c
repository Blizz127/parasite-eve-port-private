typedef struct {
    unsigned char *p;
    short id;
    short pad;
} Ent;

extern Ent D_800BE830[];
extern unsigned char D_8009D1D4;
extern unsigned char D_8009CE3C;
extern signed char D_8009D2A0;
extern unsigned int D_8009D1A0;
extern unsigned char *D_8009D278;
extern unsigned char *D_8009D254;
extern unsigned char *D_8009D20C;
extern short D_8009D27C;
extern unsigned char D_8009CE38;
extern unsigned char D_8009CE39;
extern unsigned char D_8009CE3A;
extern unsigned char D_8009CE3B;
extern int D_8009D258;
extern unsigned char *D_800942E4;

extern void func_80053D2C(int);
extern short func_80030584(unsigned char *, unsigned char *);
extern void func_8001A680(unsigned char *, int);
extern void func_80070064(void);
extern void func_800518A8(unsigned char *);
extern int func_80030534(unsigned char *, unsigned char *);
extern int func_80079FB4(int, int);
extern int func_8006F39C(int, unsigned char *);
extern void func_8006DDCC(int, int, int, int, int);
extern signed char func_80024A3C(void);
extern void func_80028C48(unsigned char *);
extern void func_80020D50(void);
extern void func_8002312C(Ent *);
extern void func_80024250(int, unsigned char *);
extern void func_80021128(void);

typedef struct { int f; } SW;
typedef struct { short f; } SH;
#define W(p, o) (((SW *)((unsigned char *)(p) + (o)))->f)
#define WP(p, o) (*(int *)((unsigned char *)(p) + (o)))
#define H(p, o) (((SH *)((unsigned char *)(p) + (o)))->f)

void func_80022394(void)
{
    unsigned char n;
    unsigned char i;
    Ent *e;
    int k;
    int v;

    n = D_8009D1D4;
    e = &D_800BE830[n];
    if (D_8009D2A0 == 0) {
        for (i = n; i < D_8009CE3C; i++) {
            if (D_800BE830[i].id >= 3 && D_800BE830[i].id < 0x183) {
                func_80053D2C(D_800BE830[i].id - 3);
            }
        }
        D_8009CE3C = 0;
        D_8009D1D4 = 0;
        D_8009D1A0 &= ~0x100;
        return;
    }
    if (W(D_8009D278, 0x4C) & 0x200000) {
        unsigned char tbl[20] = { 0x5C, 0x5D, 0x5E, 0x64, 0x68, 0x67, 0x6A, 0x65, 0x63, 0x62, 0x66, 0x60, 0x61, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5F, 0x63 };
        unsigned short snd[20] = { 0x492, 0x494, 0x496, 0x498, 0x49A, 0x49C, 0x49E, 0x4A0, 0x4A2, 0x4A4, 0x4A6, 0x4A8, 0x4AA, 0x492, 0x492, 0x492, 0x492, 0x492, 0x4AC, 0x4B1 };
        unsigned char found;
        unsigned char *ptr;
        unsigned char *p;
        unsigned char j;

        k = e->id - 0x183;
        if (e->p != D_8009D254) {
            H(D_8009D254, 0x3A) = func_80030584(e->p + 0x1B4, D_8009D254 + 0x28);
            j = D_8009D1D4;
            found = 0;
            if (D_800BE830[j].id - 0x183 == 0x13) {
                ptr = 0;
                for (i = j; i < j + 7; i++) {
                    p = D_800BE830[i].p;
                    if (p != 0 && W(W(p, 0), 0x10) > 0 && !(W(p, 0x98) & 0x4000)) {
                        ptr = p;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    D_8009D1D4 += 7;
                    if (!(D_800BE830[D_8009D1D4].id >= 3 && D_800BE830[D_8009D1D4].id < 0x197) || D_8009D254[0xE] < 4) {
                        D_8009D1A0 &= ~0x100;
                    }
                    func_8001A680(D_8009D254, D_8009D278[0x12]);
                    W(D_8009D278, 0x4C) |= 0x200000;
                    return;
                }
                if (ptr != 0) {
                    j = D_8009D1D4;
                    for (i = j; i < j + 7; i++) {
                        p = D_800BE830[i].p;
                        if (W(W(p, 0), 0x10) <= 0 || p == 0) {
                            D_800BE830[i].p = ptr;
                        }
                    }
                }
            } else {
                p = D_800BE830[j].p;
                if (p == 0 || W(W(p, 0), 0x10) <= 0 || (W(p, 0x98) & 0x4000)) {
                    D_8009D1D4 += 1;
                    if (!(D_800BE830[D_8009D1D4].id >= 3 && D_800BE830[D_8009D1D4].id < 0x197) || D_8009D254[0xE] < 4) {
                        D_8009D1A0 &= ~0x100;
                    }
                    func_8001A680(D_8009D254, D_8009D278[0x12]);
                    W(D_8009D278, 0x4C) |= 0x200000;
                    return;
                }
            }
        }
        func_80070064();
        if (k == 6) {
            int s;
            int r;
            int sel;

            unsigned char *o = (unsigned char *)W(D_8009D278, 0x68);

            if (!(W(o, 0xC) & 0x3FF)) {
                func_800518A8(o);
            }
            W(D_8009D278, 0x4C) |= 0x100000;
            s = H(e->p, 0x26A) - D_8009D27C;
            r = func_80079FB4(s, func_80030534(e->p, D_8009D254));
            if (r < -0xAB) {
                sel = 0;
            } else if (r < 0xE4) {
                sel = 1;
            } else {
                sel = 2;
            }
            func_8001A680(D_8009D254, D_8009D278[sel + 0x14]);
            WP(D_8009D278, 0x4C) &= ~0x200000;
            D_8009CE39 = 0x58;
            D_8009CE38 = 0;
            D_8009CE3A = 0x18;
            D_8009CE3B = 0;
        } else {
            func_8001A680(D_8009D254, 0xF);
        }
        if (k != 0x13) {
            D_8009D258 = func_8006F39C(tbl[k], e->p);
        }
        if (k == 7 || k == 8 || k == 10) {
            func_8006DDCC(snd[k], 0, H(e->p, 0x268), H(e->p, 0x26A), H(e->p, 0x26C));
        } else {
            func_8006DDCC(snd[k], 0, H(D_8009D254, 0x2A), H(D_8009D254, 0x2E), H(D_8009D254, 0x32));
        }
        D_8009D1A0 |= 0x100;
        W(D_8009D278, 0x4C) &= ~0x200000;
    }
    if (W(D_8009D278, 0x4C) & 0x80000) {
        unsigned char *q;

        if (func_80024A3C() == 0) {
            return;
        }
        W(D_8009D278, 0x8) = 0;
        W(D_8009D254, 0x98) &= ~0x100;
        for (i = D_8009D1D4 - 7; i < D_8009D1D4; i++) {
            if (D_800BE830[i].p != D_8009D254) {
                func_80028C48(D_800BE830[i].p);
            }
        }
        for (q = D_8009D20C; q != 0; q = (unsigned char *)W(q, 4)) {
            if (q != D_8009D254 && W(q, 0) != 0 && W(W(q, 0), 0x18) != 0) {
                *(unsigned char *)W(W(q, 0), 0x18) = 4;
                W(W(q, 0), 0) &= 0xC0FFFFFF;
            }
        }
        W(D_8009D278, 0x4C) &= ~0x80000;
        func_80020D50();
        return;
    }
    if (W(D_8009D278, 0x4C) & 0x100000) {
        H(D_8009D254, 0x3A) = func_80030584(e->p + 0x1B4, D_8009D254 + 0x28);
        func_8002312C(e);
        if (D_800942E4[D_8009D258 * 0xA0C] == 1 || D_800942E4[D_8009D258 * 0xA0C] == 2) {
            return;
        }
        W(D_8009D278, 0x8) = 0;
        W(D_8009D254, 0x98) &= ~0x100;
        D_8009D1A0 &= ~0x100;
        W(D_8009D278, 0x4C) = (W(D_8009D278, 0x4C) & ~0x100000) | 0x200000;
        func_80020D50();
        return;
    }
    if (D_8009D254[0xF] == *(unsigned short *)(D_8009D254 + 0x1A)) {
        func_8001A680(D_8009D254, D_8009D278[0x12]);
    }
    if ((D_800942E4[D_8009D258 * 0xA0C] == 1 || D_800942E4[D_8009D258 * 0xA0C] == 2) && e->id != 0x196) {
        return;
    }
    W(D_8009D278, 0x4C) |= 0x200000;
    func_80024250(e->id - 0x183, e->p);
    if (e->id == 0x196) {
        return;
    }
    D_8009D1D4 += 1;
    if (!(D_800BE830[D_8009D1D4].id >= 3 && D_800BE830[D_8009D1D4].id < 0x197) || D_8009D254[0xE] < 4) {
        D_8009D1A0 &= ~0x100;
    }
    for (i = D_8009D1D4; i < D_8009CE3C; i++) {
        if (D_800BE830[i].id == 1 || D_800BE830[i].id == 2) {
            func_80021128();
            return;
        }
    }
}
