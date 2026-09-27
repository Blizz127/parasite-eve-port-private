extern void (*D_8009B728)(unsigned char *a0);
extern int func_80084F8C(unsigned char *a0);
extern int func_80083644(unsigned char *a0);

void func_80084C4C(unsigned char *a0)
{
    int old;
    register int v asm("$2");
    register int fifteen asm("$3");
    int i;
    int s;
    int (*f)(unsigned char *a0);
    register int r asm("$2");

    fifteen = 0xF;
    old = a0[0xE8];
    v = **(unsigned char **)(a0 + 0x3C) >> 4;
    a0[0xE8] = v;
    if (v == fifteen) {
        a0[0xE8] = old;
    } else {
        **(unsigned char **)(a0 + 0x30) = 0;
        (*(unsigned char **)(a0 + 0x30))[1] = **(unsigned char **)(a0 + 0x3C);
        i = 2;
        if (i < a0[0x44]) {
            do {
                (*(unsigned char **)(a0 + 0x30))[i] =
                    (*(unsigned char **)(a0 + 0x3C))[i];
                i++;
            } while (i < a0[0x44]);
        }
    }
    if ((*(unsigned char **)(a0 + 0x3C))[1] == 0) {
        if (a0[0x46] == 1) {
            if (*(int *)(a0 + 0x14) == 0) {
                goto check;
            }
        }
        if (a0[0x50] == 0) {
            goto notify;
        }
    }
check:
    if (func_80084F8C(a0) == 0) {
        if (a0[0x37] == 0) {
            if (a0[0x4A] == 0) {
                if (a0[0xE8] != old) {
notify:
                    D_8009B728(a0);
                }
            }
        }
    }
    a0[0x4A] = 0;
    if ((unsigned int)((a0[0x46] - 2) & 0xFF) < 0xFC) {
        if (**(unsigned char **)(a0 + 0x3C) != 0xF3) {
            D_8009B728(a0);
        }
    }
    s = a0[0x46];
    if (s != 0) {
        if (s != 0xFF) {
            if (a0[0x36] == 0) {
                return;
            }
        }
    }
    switch (s) {
    case 0:
        if (a0[0xE8] == 0) {
            return;
        }
        v = a0[0x46];
        a0[0x49] = 1;
        goto bump;
    case 1:
        v = a0[0x46];
        a0[0x47] = 0;
bump:
        a0[0x46] = v + 1;
        return;
    case 0xFE:
        a0[0x46] = 0xFF;
        return;
    case 0xFF:
        return;
    }
    f = *(int (**)(unsigned char *))(a0 + 0x18);
    if (f != 0) {
        r = f(a0);
    } else {
        r = func_80083644(a0);
    }
    a0[0x46] = a0[0x46] + r;
}
