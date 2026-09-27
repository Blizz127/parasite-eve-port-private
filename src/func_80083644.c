extern int func_80083790(unsigned char *);
extern void func_800837C8(unsigned char *, unsigned char *);

int func_80083644(unsigned char *a0)
{
    int c;

    switch (a0[0x46]) {
    case 2:
        a0[0xE3] = (*(unsigned char **)(a0 + 0x3C))[3];
        a0[0xE4] = (*(unsigned char **)(a0 + 0x3C))[4];
        *(short *)(a0 + 0xE6) = 0;
        a0[0xE9] = (*(unsigned char **)(a0 + 0x3C))[5];
        a0[0xEA] = (*(unsigned char **)(a0 + 0x3C))[6];
        *(int *)(a0 + 0xEC) = 0;
        break;
    case 3:
        {
            unsigned char *p = *(unsigned char **)(a0 + 0x3C);
            int hi = p[4];
            int lo = p[5];

            a0[0x47] = 0;
            *(short *)(a0 + 0xE6) = lo + (hi << 8);
        }
        break;
    case 4:
        {
            register int bb asm("$3");
            register unsigned char *p asm("$3");
            register int ee asm("$4");
            register int cc asm("$2");

            ee = *(int *)(a0 + 0xEC);
            cc = a0[0x47];
            p = *(unsigned char **)(a0 + 0x3C);
            cc = cc + 1;
            bb = p[4];
            ee = ee + 8;
            a0[0x47] = cc;
            *(int *)(a0 + 0xEC) = ee + ((bb + 3) & 0x1FC);
            c = cc;
        }
        if ((unsigned char)c < a0[0xEA]) {
            return 0;
        }
        if (func_80083790(a0) > 0x80) {
            a0[0x46] = 0xFE;
            a0[0x49] = 2;
        } else {
            a0[0x46] = 0xFF;
            func_800837C8(a0, a0 + 0x63);
            a0[0x46] = 2;
        }
        return 0;
    default:
        return 1;
    }
    return 1;
}
