extern unsigned char *D_800A5AD0;

typedef struct {
    unsigned short *tab;      /* 0x00 */
    unsigned char *rec5;      /* 0x04 */
    unsigned char *rec8;      /* 0x08 */
    unsigned char pad0C[0x30];
    unsigned char *src;       /* 0x3C */
    unsigned char pad40[6];
    unsigned char state;      /* 0x46 */
    unsigned char idx;        /* 0x47 */
    unsigned char cnt;        /* 0x48 */
    unsigned char next;       /* 0x49 */
    unsigned char pad4A[0xE3 - 0x4A];
    unsigned char n2;         /* 0xE3 */
    unsigned char padE4[5];
    unsigned char n3;         /* 0xE9 */
    unsigned char n4;         /* 0xEA */
} Pad;

int func_80083944(Pad *p)
{
    unsigned char *e;
    unsigned char *s;
    unsigned char *v;
    int n;
    unsigned char *e4;
    unsigned char *s4;

    switch (p->state) {
    case 2:
        p->tab[p->idx] = p->src[5] + (p->src[4] << 8);
        if (++p->idx >= p->n2) {
            p->idx = 0;
            goto one;
        }
        return 0;
    case 3:
        s = p->src;
        e = p->rec5 + p->idx * 5;
        e[0] = s[4];
        e[1] = p->src[5] & 0x7F;
        e[2] = p->src[6];
        e[3] = p->src[7];
        {
            int w = p->src[5];
            asm("" : "=r"(w) : "0"(w));
            e[4] = w >> 7;
        }
        if (++p->idx >= p->n3) {
            p->idx = 0;
            p->cnt = 0;
            goto one;
        }
        return 0;
    case 4:
        e4 = p->rec8 + p->idx * 8;
        if (p->cnt == 0) {
            e4[0] = p->cnt = p->src[4];
            n = 3;
            s4 = p->src + 5;
            if (p->idx == 0) {
                v = p->rec8 + p->n4 * 8;
            } else {
                v = *(unsigned char **)(e4 - 4) + ((e4[-8] + 3) & 0x1FC);
            }
            *(unsigned char **)(e4 + 4) = v;
            D_800A5AD0 = v;
        } else {
            n = 6;
            s4 = p->src + 2;
        }
        n--;
        if (n != -1) {
            do {
                if (p->cnt == 0) {
                    n--;
                    goto next;
                }
                *D_800A5AD0++ = *s4++;
                n--;
                p->cnt--;
            } while (n != -1);
        }
        if (p->cnt != 0) {
            return 0;
        }
    next:
        if (++p->idx >= p->n4) {
            p->next = 6;
            p->state = 0xFE;
            return 0;
        }
        p->cnt = 0;
        return 0;
    }
one:
    return 1;
}
