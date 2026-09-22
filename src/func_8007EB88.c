typedef struct E2 {
    int f0;
    unsigned char f4;
    unsigned char f5[11];
} E2;

extern E2 D_800A3610[];
extern int D_800A3690;
extern void func_80080998(unsigned char *, int);

void func_8007EB88(int a0, int a1, int a2)
{
    int *idx = &D_800A3690;
    unsigned char *base;
    int i;

    asm volatile("" : "=r"(idx) : "0"(idx));
    D_800A3610[*idx].f0 = a0;
    D_800A3610[*idx].f4 = a1;
    base = (unsigned char *)idx - 0x7B;
    func_80080998((unsigned char *)((*idx << 4) + (int)base), a2);
    i = *idx + 1;
    *idx = i;
    if (i >= 8) {
        *idx = 0;
    }
}
