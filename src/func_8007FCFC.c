extern unsigned char D_8009B558[];
extern unsigned char D_8009B598[];
extern int D_8009B5A4[];
extern void func_8007B9EC(void);
extern void func_80080950(unsigned char *, unsigned char *);
extern int func_8007B558(int, unsigned char *, int, int);

int func_8007FCFC(unsigned char a0, unsigned char *a1)
{
    unsigned char *s;
    unsigned char *p;
    unsigned char *t;

    func_8007B9EC();
    s = D_8009B558;
    s[0] = a0;
    if (a1 != 0) {
        func_80080950(s + 1, a1);
        *(unsigned char **)(s + 8) = s + 1;
    } else {
        *(unsigned char **)(s + 8) = 0;
    }
    p = D_8009B598;
    *(int *)p = D_8009B5A4[p[-0x40]] ? 0x3C0 : 0x1E;
    *(int *)(p + 4) = 0;
    switch (p[-0x40]) {
    case 7:
        if (p[-0xD] == 1) {
            p[-0x40] = 1;
            *(int *)(p - 0x38) = 0;
        }
        break;
    case 8:
        if (p[-0xD] != 1) {
            p[-0x40] = 1;
            *(int *)(p - 0x38) = 0;
        }
        break;
    }
    t = D_8009B558;
    if (func_8007B558(t[0], *(unsigned char **)(t + 8), 0, 1) != 0) {
        *(int *)(t + 0x44) = 0;
        *(int *)(t + 0x40) = 0;
        return 0;
    }
    t[0x28] = t[0];
    return 1;
}
