typedef struct {
    int w0;
    unsigned char b4;
    unsigned char b5;
    unsigned char b6;
    unsigned char b7;
    unsigned char b8;
    unsigned char pad[3];
    int wC;
    int w10;
    int w14;
} Rec7E5C4;

extern Rec7E5C4 D_800A3540[];
extern int D_800A3600;
extern int D_800A3604;

void func_8007E5C4(void)
{
    register int *b asm("$2");
    register int *s asm("$5");
    register int *cnt asm("$8");
    register Rec7E5C4 *tbl asm("$7");
    register int first asm("$6");
    int idx;

    b = &D_800A3600;
    asm volatile("" : "=r"(b) : "0"(b));
    cnt = b + 2;
    tbl = (Rec7E5C4 *)((unsigned char *)b - 0xC0);
    idx = *b;
    asm volatile("" : "=r"(idx) : "0"(idx));
    s = b;
    asm volatile("" : "=r"(s) : "0"(s));
    first = D_800A3540[idx].w0;
test:
    if (*cnt <= 0) {
        goto exit;
    }
    {
        register unsigned char *r asm("$2");
        register unsigned char *q asm("$3");
        int i;
        int n;
        int ix;

        i = 3;
        r = (unsigned char *)(D_800A3600 * 24);
        r = (unsigned char *)((int)r + (int)tbl);
        q = r + 3;
        *(int *)r = 0;
        r[4] = 0;
        do {
            q[5] = 0;
            i--;
            q--;
        } while (i >= 0);
        *(int *)(r + 0xC) = 0;
        *(int *)(r + 0x10) = 0;
        *(int *)(r + 0x14) = 0;
        n = *s + 1;
        *s = n;
        if (n >= 8) {
            *s = 0;
        }
        n = s[2];
        ix = *s;
        s[2] = n - 1;
        if (D_800A3540[ix].w0 == first) {
            goto test;
        }
    }
exit:
    {
        register int *p asm("$3");

        p = &D_800A3604;
        asm volatile("" : "=r"(p) : "0"(p));
        *p = p[-1];
    }
}
