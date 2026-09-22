extern unsigned int *D_8009B784;
extern void (*D_8009B74C)();

int func_80082B08(void)
{
    unsigned int *p;

    p = D_8009B784;
    if ((p[1] & 1) == 0) {
        return 0;
    }
    if ((p[0] & 1) == 0) {
        return 0;
    }
    if (D_8009B74C != 0) {
        D_8009B74C();
    }
    return 1;
}
