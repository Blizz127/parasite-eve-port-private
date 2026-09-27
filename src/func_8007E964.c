extern int D_800A3604;
extern int D_800A3608;
extern unsigned char D_800A3510[];
extern void (*D_800B8AB0)(int, unsigned char *);
extern void func_80080998(unsigned char *, unsigned char *);
extern void func_8007EB88(int, int, unsigned char *);
extern void func_8007E5C4(void);
extern int func_8007FBF0(int);
extern void func_8007FB44(unsigned char, int);

void func_8007E964(int a0, unsigned char *a1)
{
    register int intr asm("$6");
    register unsigned char *result asm("$17");
    register int *headp asm("$18");
    register unsigned char *base asm("$19");
    register int code asm("$20");
    int head;
    int n;
    int inrange;
    int k;
    int com;
    int retry;
    void (*cb)(int, unsigned char *);
    int st;
    int *cnt;
    unsigned char *e;
    unsigned char *q;

    intr = a0;
    result = a1;
    headp = &D_800A3604;
    head = *headp;
    base = (unsigned char *)headp - 0xC4;
    q = base + head * 24;
    com = *(int *)q;
    code = intr;
    if (com != 0) {
        unsigned char *last;

        last = D_800A3510;
        *(int *)last = com;
        *(unsigned char *)(last + 4) = intr;
        func_80080998(last + 5, result);
        if ((code & 0xFF) == 2) {
            goto case2;
        }
        if ((code & 0xFF) == 5) {
            goto case5;
        }
        goto joined;
        case2:
            n = *headp + 1;
            inrange = n < 8;
            k = n;
            if (inrange == 0) {
                k = 0;
            }
            if (*(int *)(base + k * 24) != *(int *)q) {
                func_8007EB88(*(int *)q, 2, result);
                cb = *(void (**)(int, unsigned char *))(q + 0x10);
                if (cb != 0) {
                    cb(2, result);
                }
                func_8007E5C4();
            } else {
                *headp = n;
                if (inrange == 0) {
                    *headp = 0;
                }
            }
            goto joined;
        case5:
            retry = *(int *)(q + 0x14);
            if (retry > 0 || retry == -1) {
                *headp = *(int *)(base + 0xC0);
                retry = *(volatile int *)(q + 0x14);
                if (retry != -1) {
                    *(int *)(q + 0x14) = (int)((unsigned)retry + 0xFFFFFFFFu);
                }
            } else {
                func_8007EB88(*(int *)q, 5, result);
                cb = *(void (**)(int, unsigned char *))(q + 0x10);
                if (cb != 0) {
                    cb(5, result);
                }
                func_8007E5C4();
            }
        joined:
            ;
    }
    if (D_800B8AB0 != 0) {
        D_800B8AB0(code & 0xFF, result);
    }
    st = func_8007FBF0(0);
    if (st == 1) {
        register int h asm("$3");
        register int scaled asm("$2");

        result = (unsigned char *)&D_800A3608;
        if (*(int *)result > 0 && func_8007FBF0(0) == st) {
            h = D_800A3604;
            scaled = h * 24;
            h = (int)result - 0xC8;
            e = (unsigned char *)(scaled + h);
            if (*(int *)e != 0) {
                func_8007FB44(*(unsigned char *)(e + 4), *(int *)(e + 0xC));
            }
        }
    }
}
