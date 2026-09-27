extern int D_800956C0[];
extern volatile int *D_800956BC;

int func_800746A0(int channel, int handler)
{
    register unsigned int r2 asm("$2");
    register unsigned int r3 asm("$3");
    register unsigned int r4 asm("$4");
    register unsigned int r5 asm("$5");
    register unsigned int r6 asm("$6");
    register unsigned int r7 asm("$7");

    r6 = channel;
    r3 = (unsigned int)D_800956C0;
    r2 = r6 << 2;
    r3 = r3 + r2;
    r7 = *(int *)r3;
    r4 = handler;
    r2 = r7;
    if (r4 == r7) {
        goto done;
    }
    r2 = 0x00FF0000;
    if (r4 == 0) {
        goto remove;
    }
    r5 = (unsigned int)D_800956BC;
    r2 = r2 | 0xFFFF;
    *(int *)r3 = r4;
    r4 = *(volatile int *)r5;
    r3 = r6 + 16;
    r4 = r4 & r2;
    r2 = 1;
    r2 = r2 << r3;
    r3 = 0x00800000;
    r2 = r2 | r3;
    r4 = r4 | r2;
    *(volatile int *)r5 = r4;
    r2 = r7;
    goto done;
remove:
    r5 = (unsigned int)D_800956BC;
    r2 = r2 | 0xFFFF;
    asm volatile("" : "=r"(r4) : "0"(r4));
    *(int *)r3 = 0;
    r3 = *(volatile int *)r5;
    r4 = r6 + 16;
    r3 = r3 & r2;
    r2 = 0x00800000;
    r3 = r3 | r2;
    r2 = 1;
    r2 = r2 << r4;
    r2 = ~r2;
    r3 = r3 & r2;
    *(volatile int *)r5 = r3;
    r2 = r7;
    asm volatile("" : "=r"(r2) : "0"(r2));
done:
    return r2;
}
