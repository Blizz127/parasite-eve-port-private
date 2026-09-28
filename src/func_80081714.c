typedef struct {
    int num;                       /* 0x00 */
    int parent;                    /* 0x04 */
    int pos;                       /* 0x08 */
    char name[32];                 /* 0x0C */
} DsDir;                           /* 0x2C */

extern int D_8009B6DC;
extern int D_8009AFC0;
extern DsDir D_800A3CB0[];
extern unsigned char D_800A52B0[];
extern char D_80011EF8[];
extern char D_80011F24[];
extern char D_80011F2C[];
extern char D_80011F5C[];
extern char D_80011F80[];
extern char D_80011FA0[];
extern char D_80011FB4[];
extern int func_80081D18(int, int, unsigned char *);
extern void func_80073C5C(char *);
extern int func_80071A04(unsigned char *, char *, int);
extern void func_80071A34(char *, unsigned char *, int);
extern int func_80071A74(char *, ...);

int func_80081714(void) {
    unsigned char *p;
    int i;
    int n;
    int loc;
    char *name;
    unsigned char *buf;

    if (func_80081D18(1, 16, D_800A52B0) != 1) {
        if (D_8009AFC0 > 0) {
            func_80073C5C(D_80011EF8);
        }
        return 0;
    }
    buf = D_800A52B0;
    if (func_80071A04(buf + 1, D_80011F24, 5) != 0) {
        if (D_8009AFC0 > 0) {
            func_80073C5C(D_80011F2C);
        }
        return 0;
    }
    __asm__("" : "=r"(buf) : "0"(buf));
    __builtin_memcpy(&loc, buf + 140, 4);
    if (func_80081D18(1, loc, buf) != 1) {
        if (D_8009AFC0 > 0) {
            func_80071A74(D_80011F5C, loc);
        }
        return 0;
    }
    p = buf;
    if (D_8009AFC0 >= 2) {
        func_80073C5C(D_80011F80);
    }
    i = 0;
    if (p < buf + 2048) {
        do {
        if (*p == 0) {
            goto out;
        }
        __builtin_memcpy(&D_800A3CB0[i].pos, p + 2, 4);
        name = D_800A3CB0[i].name;
        {
            int par = p[6];
            n = i + 1;
            D_800A3CB0[i].num = n;
            D_800A3CB0[i].parent = par;
        }
        func_80071A34(name, p + 8, p[0]);
        name[p[0]] = 0;
        p += p[0] + 8 + (p[0] & 1);
        if (D_8009AFC0 >= 2) {
            func_80071A74(D_80011FA0, D_800A3CB0[i].pos, D_800A3CB0[i].num,
                          D_800A3CB0[i].parent, name);
        }
        i = n;
        /* The sector end is a statement-expression temporary: its block notes
         * stretch the temp's loop.c lifetime to 4 luids, so loop.c hoists it
         * as the LAST movable (after &D_800A3CB8 / +4) and cse2 turns it into
         * the `addu $s5,$v1,$zero` copy of the guard temp, as in retail. */
        } while (i < 128 && p < ({
            unsigned char *e_ = buf + 2048;
            { int z_; }
            e_;
        }));
    }
out:
    if (i < 128) {
        D_800A3CB0[i].parent = 0;
    }
    D_8009B6DC = 0;
    if (D_8009AFC0 >= 2) {
        func_80071A74(D_80011FB4, i);
    }
    return 1;
}
