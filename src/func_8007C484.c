extern int D_800BE9EC;
extern int D_800C20C4;
extern int D_800C0DBC;
extern unsigned char *D_800C0DC8;

int func_8007C484(unsigned char **a0, unsigned short **a1)
{
    unsigned short *p;

    p = (unsigned short *)(D_800C0DC8 + (D_800BE9EC << 5));
    if (*p == 1) {
        D_800BE9EC = 0;
        if (D_800C0DBC != 0) {
            *p = 0;
        }
        p = (unsigned short *)(D_800C0DC8 + (D_800BE9EC << 5));
    }
    asm volatile("" : "=r"(p) : "0"(p));
    if (*p == 2) {
        *p = 4;
        *a0 = D_800C0DC8 + (D_800C20C4 << 5) + D_800BE9EC * 0x7E0;
        *a1 = p;
        return 0;
    }
    return 1;
}
