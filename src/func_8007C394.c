typedef struct Ent {
    short f0;
    short f2;
    short f4;
    unsigned short f6;
    char pad8[24];
} Ent;

extern Ent *volatile D_800C0DC8;
extern int D_800C20C4;
extern int D_800BE9EC;

int func_8007C394(int *a0)
{
    Ent *b;
    register int f0v asm("$2");
    register unsigned short raw asm("$3");
    Ent *q;
    int i;
    register int j asm("$4");
    register int n asm("$2");

    b = D_800C0DC8;
    i = (a0 - (int *)(b + D_800C20C4)) / 504;
    b = &b[i];
    f0v = b->f0;
    raw = b->f6;
    if (f0v != 4) {
        return 1;
    }
    asm volatile("" : "=r"(raw) : "0"(raw));
    n = (short)raw;
    j = 0;
    if (n > 0) {
        register int cnt asm("$6") = n;
        do {
            int k = j + i;
            j++;
            q = D_800C0DC8;
            q += k;
            q->f0 = 0;
        } while (j < cnt);
    }
    n = j + i;
    D_800BE9EC = n;
    return 0;
}
