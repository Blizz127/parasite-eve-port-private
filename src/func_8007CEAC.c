extern unsigned char *D_8009B348;
extern unsigned int *volatile D_8009B344;
extern volatile unsigned char *D_8009B32C;
extern char D_80011C2C[];
extern void func_80071A74(char *, unsigned int);

void func_8007CEAC(int ch, unsigned int addr, unsigned int blocks, unsigned int words,
                   unsigned int control, unsigned char intr)
{
    volatile unsigned int t;
    unsigned int i;
    unsigned char *p;
    int v;
    unsigned int bit;
    unsigned int *q;
    volatile unsigned int *dma;

    i = 0;
    while (*(volatile unsigned int *)(0x1F801088 + (ch << 4)) & 0x01000000) {
        if (i == 0x10000) {
            func_80071A74(D_80011C2C, *(volatile unsigned int *)(0x1F801088 + (ch << 4)));
            break;
        }
        i++;
    }
    if (intr == 1) {
        p = D_8009B348;
        i = p[2]; p[2] = i | (1 << ch);
        t = *(unsigned int *)D_8009B348;
    } else {
        p = D_8009B348;
        i = p[2]; p[2] = i & ~(1 << ch);
        t = *(unsigned int *)D_8009B348;
    }
    do { v = ch << 2; v += 3; } while (0);
    bit = 1;
    bit <<= v;
    q = D_8009B344;
    dma = (volatile unsigned int *)(0x1F801080 + (ch << 4));
    v = *q;
    v |= bit;
    *q = v;
    *dma = addr;
    dma++;
    *dma = (blocks << 16) | words;
    dma++;
    while ((*D_8009B32C & 0x40) == 0) {
    }
    *dma = control;
    t = *dma; __asm__ volatile("");
}
