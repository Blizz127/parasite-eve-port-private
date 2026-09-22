extern int D_800956AC;
extern void (*D_8009568C[8])();

void func_8007440C(void)
{
    register int i asm("$17");
    register void (**p)() asm("$16");

    D_800956AC = D_800956AC + 1;
    i = 0;
    p = D_8009568C;
    for (; i < 8; i++) {
        if (*p != 0) {
            (*p)();
        }
        p++;
    }
}
