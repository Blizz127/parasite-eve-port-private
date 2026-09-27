typedef struct Rec {
    unsigned int f0;
    unsigned char pad4[0xCC - 4];
    unsigned int fCC;
} Rec;

typedef struct Ent {
    Rec *f0;
    unsigned char pad4[0x2A - 4];
    short f2A;
    unsigned char pad2C[2];
    short f2E;
    unsigned char pad30[2];
    short f32;
    unsigned char pad34[0x98 - 0x34];
    int f98;
} Ent;

typedef struct Cfg {
    unsigned char pad0[4];
    unsigned int f4;
} Cfg;

typedef struct St {
    unsigned char pad0[8];
    int f8;
    short fC;
    unsigned char padE[0x1C - 0xE];
    short f1C;
    unsigned char pad1E[0x28 - 0x1E];
    int f28;
    unsigned char pad2C[0x4C - 0x2C];
    unsigned int f4C;
    unsigned char pad50[0x6C - 0x50];
    Cfg *f6C;
} St;

#define FL (*(unsigned int *)((char *)D_8009D278 + 0x4C))

extern unsigned int D_8009D1A0;
extern Ent *D_8009D254;
extern St *D_8009D278;
extern unsigned int D_8009D2E8;
typedef struct Flags {
    unsigned int cnt : 8;
    unsigned int mode : 2;
    unsigned int v : 2;
    unsigned int b12 : 1;
    unsigned int b13 : 1;
    unsigned int b14 : 1;
    unsigned int b15 : 1;
    unsigned int b16 : 1;
    unsigned int b17 : 1;
    unsigned int b18 : 1;
    unsigned int rest : 13;
} Flags;

extern Flags D_8009D1AC;
extern Ent *D_8009D1A8;
extern short D_8009D228;

extern void func_8006DDCC(int, int, int, int, int);
extern int func_80071A54(void);
extern void func_8001A680(Ent *, unsigned short);

void func_80024250(unsigned int kind, Ent *e)
{
    int dmg;
    Rec *r;

    dmg = 0;
    if (!(D_8009D1A0 & 2)) {
        short tbl[20] = {0x492, 0x494, 0x496, 0x498, 0x49A, 0x49C, 0x49E, 0x4A0, 0x4A2, 0x4A4,
                         0x4A6, 0x4A8, 0x4AA, 0x492, 0x492, 0x492, 0x492, 0x492, 0x4AC, 0x4B1};

        func_8006DDCC((unsigned short)tbl[kind], 1, D_8009D254->f2A, D_8009D254->f2E, D_8009D254->f32);
        D_8009D278 = (St *)D_8009D254->f0;
    }
    switch (kind) {
    case 0:
        D_8009D278->fC += 0x1E;
        dmg = 0x3C0000;
        break;
    case 1:
        D_8009D278->fC += 0x3C;
        dmg = 0x780000;
        break;
    case 2:
        D_8009D278->fC += 0x118;
        dmg = 0x1F40000;
        break;
    case 3:
        if ((FL & 3) != 3) {
            FL &= ~3;
        }
        dmg = 0x640000;
        break;
    case 4:
        if ((FL & 3) != 3) {
            FL &= ~3;
        }
        if ((FL & 0xC) != 0xC) {
            FL &= ~0xC;
            D_8009D2E8 &= ~0x10;
        }
        if ((FL & 0x30) != 0x30) {
            FL &= ~0x30;
        }
        if ((FL & 0xC0) != 0xC0) {
            FL &= ~0xC0;
        }
        dmg = 0x2580000;
        FL &= ~0x1000;
        break;
    case 5:
        FL |= 0x200;
        D_8009D278->f8 -= D_8009D278->f28 / 3;
        break;
    case 7:
        r = e->f0;
        D_8009D1AC.b12 = (r->fCC & 3) == 2;
        D_8009D1AC.b13 = (r->fCC & 0xC) == 8;
        D_8009D1AC.b14 = (r->fCC & 0x30) == 0x20;
        D_8009D1AC.b15 = (r->fCC & 0xC000) == 0x8000;
        D_8009D1AC.b16 = (r->fCC & 0x30000) == 0x20000;
        D_8009D1AC.b17 = (r->fCC & 0x3000) == 0x2000;
        D_8009D1AC.b18 = (r->fCC & 0xC00) == 0x800;
        if (((e->f0->fCC >> 6) & 3) == 2) {
            D_8009D1AC.v = 1;
        } else {
            D_8009D1AC.v = 0;
        }
        D_8009D1AC.cnt = 0x4B;
        D_8009D1A8 = e;
        D_8009D1AC.mode = 1;
        dmg = 0x320000;
        break;
    case 8:
        switch ((e->f0->fCC >> 8) & 3) {
        case 0:
            if (func_80071A54() & 1) {
                break;
            }
        case 2:
            e->f0->f0 |= 1;
            break;
        }
        dmg = 0x5A0000;
        break;
    case 9:
        D_8009D278->f4C |= 0x100;
        D_8009D228 = 0x1C2;
        if ((D_8009D278->f4C & 0xC0) == 0x40 || (D_8009D278->f4C & 0xC0) == 0x80) {
            D_8009D278->f4C &= ~0xC0;
        }
        dmg = 0xC80000;
        break;
    case 10:
        switch ((e->f0->fCC >> 10) & 3) {
        case 0:
            if (func_80071A54() & 1) {
                break;
            }
        case 2:
            e->f0->f0 = (e->f0->f0 & ~0xE) | (((func_80071A54() % 3 + 3) & 7) << 1);
            func_8001A680(e, 4);
            e->f98 |= 0x1000;
            break;
        }
        dmg = 0x960000;
        break;
    case 11:
        FL |= 0x400;
        dmg = 0x1900000;
        break;
    case 12:
        FL |= 0x800;
        dmg = 0x3E80000;
        break;
    case 18:
        D_8009D278->fC = D_8009D278->f1C;
        if ((FL & 3) != 3) {
            FL &= ~3;
        }
        if ((FL & 0xC) != 0xC) {
            FL &= ~0xC;
            D_8009D2E8 &= ~0x10;
        }
        if ((FL & 0x30) != 0x30) {
            FL &= ~0x30;
        }
        if ((FL & 0xC0) != 0xC0) {
            FL &= ~0xC0;
        }
        dmg = 0x5140000;
        FL &= ~0x1000;
        break;
    case 19:
        FL = (FL | 0x80000) & ~0x200000;
        break;
    }
    if (D_8009D278->f1C < D_8009D278->fC) {
        D_8009D278->fC = D_8009D278->f1C;
    }
    if (!(D_8009D278->f6C->f4 & 0x200)) {
        D_8009D278->f8 -= dmg;
    } else {
        D_8009D278->f8 -= dmg * 2 / 3;
    }
}
