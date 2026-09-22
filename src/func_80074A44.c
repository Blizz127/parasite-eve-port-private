extern char D_800117E0;
extern char D_80011800;
extern unsigned char D_80095704;
extern unsigned char D_8009574C;
extern unsigned char D_8009574E;
extern unsigned int *D_80095744;
extern void (*D_80095748)();
extern unsigned short D_800957CC[];
extern unsigned short D_800957D8[];
extern void func_80071A74();
extern void func_80077A28(unsigned char *d, int c, int n);
extern void func_80073C94(void);
extern void func_80077A54(int a0);
extern int func_80077144(int a0);

int func_80074A44(int a0)
{
    register unsigned char *s asm("$16");
    int (*h)();
    unsigned char *p10;

    switch (a0 & 7) {
    case 0:
    case 3:
        func_80071A74(&D_800117E0, &D_80095704, &D_8009574C);
    case 5:
        s = &D_8009574C;
        __asm__ __volatile__("" : "=r"(s) : "0"(s));
        func_80077A28(s, 0, 0x80);
        func_80073C94();
        func_80077A54((int)D_80095744 & 0xFFFFFF);
        p10 = s + 0x10;
        s[0] = func_80077144(a0);
        __asm__ __volatile__("" ::: "memory");
        s[1] = 1;
        *(short *)(s + 4) = D_800957CC[s[0] * 2];
        *(short *)(s + 6) = D_800957D8[s[0] * 2];
        func_80077A28(p10, -1, 0x5C);
        func_80077A28(s + 0x6C, -1, 0x14);
        return s[0];
    default:
        if (D_8009574E >= 2) {
            D_80095748(&D_80011800, a0);
        }
        h = (int (*)())D_80095744[13];
        return h(1);
    }
}
