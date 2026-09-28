extern int D_8009B76C;
extern void func_80071A24(unsigned char *, int);

void func_8008486C(unsigned char *a0)
{
    int n;
    int i;
    int j;
    int f;
    int m;
    unsigned char *q;
    unsigned char *r;
    unsigned char *u;
    int t;

    func_80071A24(a0 + 0x57, 6);
    if (*(unsigned short *)(a0 + 0xE6) != 0 && *(unsigned char **)(a0 + 0x28) != 0) {
        n = 6;
        if (a0[0x34] < 7) {
            n = a0[0x34];
        }
        i = 0;
        if (a0[0xE9] != 0) do {
            f = 0;
            m = 1;
            if ((*(unsigned char **)(a0 + 4))[i * 5 + 2] != 0) {
                m = 0xFF;
            }
            q = a0 + 0x5D;
            r = *(unsigned char **)(a0 + 0x28);
            j = 0;
            if (n != 0) {
                do {
                    if (*q == i && (*r & m)) {
                        f = 1;
                        goto found;
                    }
                    q++;
                    r++;
                } while (++j < n);
            }
        found:
            if (f) {
                t = D_8009B76C + (*(unsigned char **)(a0 + 4))[i * 5 + 3];

                if (t < 0x3D) {
                    D_8009B76C = t;
                } else {
                    f = 0;
                }
            }
            if (f) {
                q = a0 + 0x5D;
                r = a0 + 0x57;
                j = 0;
                if (n != 0) {
                    do {
                        if (*q++ == i) {
                            *r = 1;
                        }
                        r++;
                    } while (++j < n);
                }
            }
        } while (++i < a0[0xE9]);
        return;
    }
    if ((unsigned int)(a0[0xE8] - 4) < 2 || a0[0xE8] == 7) {
        if (*(unsigned short *)(a0 + 0xE6) == 0 && a0[0x34] >= 2) {
            u = *(unsigned char **)(a0 + 0x28);
            if ((u[0] & 0xC0) == 0x40 && (u[1] & 1) && D_8009B76C + 10 < 0x3D) {
                a0[0x57] = a0[0x58] = 1;
                D_8009B76C += 10;
            }
            return;
        }
    }
    if (*(volatile unsigned char *)(a0 + 0xE8) == 3) {
        a0[0x57] = 1;
        return;
    }
    if (*(unsigned short *)(a0 + 0xE6) == 0) {
        int v = 1;
        j = 5;
        u = a0 + 5;
        for (; j >= 0; j--) {
            u[0x57] = v;
            u--;
        }
    }
}
