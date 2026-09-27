extern unsigned char D_800A3540[];
extern unsigned char D_800A3550[];
extern int D_800A3600;
extern int D_800A3604;
extern int D_800A3608;
extern void func_8007EB88(int, int, int);

typedef struct Pair {
    int seq;
    int cb;
} Pair;

typedef void (*Fn)(int, int);

void func_8007E704(int a0, int a1)
{
    Pair slots[8];
    register int i asm("$17");
    register int stat asm("$22");
    register int resp asm("$23");
    register int slot asm("$20");
    register int *bp asm("$3");
    Pair *cursor;
    Pair *base;
    int seq;
    int off;
    int saved;
    int prev;
    int cb;
    int count;

    resp = a1;
    stat = a0;
    i = 0;
    {
        Pair *zp;

        zp = slots;
        do {
            zp->seq = 0;
            zp->cb = 0;
            asm volatile("" : "=r"(zp) : "0"(zp));
            i++;
            zp++;
        } while (i < 8);
    }

    bp = &D_800A3600;
    asm volatile("" : "=r"(bp) : "0"(bp));
    prev = 0;
    asm volatile("" : "=r"(prev) : "0"(prev));
    saved = -1;
    asm volatile("" : "=r"(saved) : "0"(saved));
    asm volatile("" : "=r"(bp) : "0"(bp));
    count = bp[2];
    asm volatile("" : "=r"(bp) : "0"(bp));
    slot = bp[0];
    asm volatile("" : "=r"(slot) : "0"(slot));
    i = 0;
    if (count > 0) {
        base = slots;
        off = slot * 24;
        cursor = (Pair *)((char *)slots - 8);
        do {
            seq = *(int *)(D_800A3540 + off);
            if (seq != prev) {
                func_8007EB88(seq, stat & 0xFF, resp);
                prev = seq;
            }
            cb = *(int *)(D_800A3550 + off);
            if (cb != 0) {
                register int neg asm("$7");

                neg = -1;
                if (saved == neg) {
                    cursor = base;
                    saved = 0;
                    slots[0].seq = seq;
                } else if (cursor->seq != seq) {
                    cursor++;
                    saved++;
                    cursor->seq = seq;
                } else {
                    goto skip_cb;
                }
                cursor->cb = cb;
            }
        skip_cb:
            slot++;
            if (slot < 8) {
                off += 24;
            } else {
                off = 0;
                slot = 0;
            }
            i++;
        } while (i < D_800A3608);
    }
    asm volatile("" : "=r"(saved) : "0"(saved));

    {
        register unsigned char *e asm("$4");
        register unsigned char *p asm("$2");
        register int n asm("$5");
        register int j asm("$3");

        n = 0;
        e = D_800A3540;
        D_800A3608 = 0;
        D_800A3604 = 0;
        D_800A3600 = 0;
        for (; n < 8; n++) {
            j = 3;
            asm volatile("" : "=r"(j) : "0"(j));
            p = e + 3;
            *(int *)e = 0;
            e[4] = 0;
            do {
                p[5] = 0;
                j--;
                p--;
            } while (j >= 0);
            *(int *)(e + 0xC) = 0;
            *(int *)(e + 0x10) = 0;
            *(int *)(e + 0x14) = 0;
            e += 0x18;
        }
    }

    if (saved >= 0) {
        register Pair *cp asm("$16");
        int st;

        i = 0;
        cp = slots;
        st = stat & 0xFF;
        do {
            register int r asm("$5");
            Fn fn;

            r = resp;
            fn = (Fn)cp->cb;
            cp++;
            asm volatile("" : "=r"(cp) : "0"(cp));
            fn(st, r);
            i++;
            st = stat & 0xFF;
        } while (i <= saved);
    }
}
