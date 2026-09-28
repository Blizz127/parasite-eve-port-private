typedef struct {
    unsigned char pos[4];          /* 0x00 CdlLOC */
    unsigned int size;             /* 0x04 */
    char name[16];                 /* 0x08 */
} DsFile;                          /* 0x18 */

typedef struct {
    int pos;                       /* 0x00 */
    unsigned char pad[40];
} DsDir;                           /* 0x2C */

extern int D_8009B6DC;
extern int D_8009AFC0;
extern DsDir D_800A3C8C[];
extern DsFile D_800A36B0[];
extern unsigned char D_800A52B0[];
extern unsigned char D_800A5AB0[];
extern char D_80011FD8[];
extern char D_80011FF8[];
extern char D_8001201C[];
extern char D_80012038[];
extern char D_80012014[] __attribute__((aligned(4)));
extern char D_80012018[] __attribute__((aligned(4)));
extern int func_80081D18(int, int, unsigned char *);
extern void func_80073C5C(char *);
extern void func_80080B44(int, unsigned char *);
extern void func_80071A34(char *, unsigned char *, int);
extern int func_80071A74(char *, ...);

int func_80081A7C(int dir) {
    unsigned char *p;
    int i;
    int loc;
    int pos;
    int one;
    DsFile *e;
    char *name;

    if (dir == D_8009B6DC) {
        return 1;
    }
    asm volatile("" : "=r"(one) : "0"(1));
    pos = D_800A3C8C[dir].pos;
    asm volatile("" : "=r"(pos) : "0"(pos));
    p = D_800A52B0;
    if (func_80081D18(one, pos, p) != 1) {
        if (D_8009AFC0 > 0) {
            func_80073C5C(D_80011FD8);
        }
        return -1;
    }
    if (D_8009AFC0 >= 2) {
        func_80073C5C(D_80011FF8);
    }
    i = 0;
    while (p < D_800A52B0 + 2048) {
        if (*p == 0) {
            break;
        }
        __builtin_memcpy(&loc, p + 2, 4);
        func_80080B44(loc, D_800A36B0[i].pos);
        __builtin_memcpy(&D_800A36B0[i].size, p + 10, 4);
        switch (i) {
        case 0:
            __builtin_memcpy(D_800A36B0[0].name, D_80012014, 2);
            break;
        case 1:
            __builtin_memcpy(D_800A36B0[1].name, D_80012018, 3);
            break;
        default:
            name = D_800A36B0[i].name;
            func_80071A34(name, p + 33, p[32]);
            name[p[32]] = 0;
            break;
        }
        if (D_8009AFC0 >= 2) {
            func_80071A74(D_8001201C, D_800A36B0[i].pos[0], D_800A36B0[i].pos[1],
                          D_800A36B0[i].pos[2], D_800A36B0[i].size, D_800A36B0[i].name);
        }
        i++;
        p += *p;
        if (i >= 64) {
            break;
        }
    }
    D_8009B6DC = dir;
    if (i < 64) {
        D_800A36B0[i].name[0] = 0;
    }
    if (D_8009AFC0 >= 2) {
        func_80071A74(D_80012038, i);
    }
    return 1;
}
