extern int D_8010DA10[];
extern int D_8010DA50[];
extern int D_8010DA0C[];
extern int D_8010DA90[];
void func_8010C1EC(int *p, int n);

int *func_8010BEFC(int *a0)
{
    int *ret;
    int *d;
    int c;

    ret = a0;
    d = D_8010DA10;
    c = 15;
    do {
        *d++ = *a0++;
    } while (--c != -1);
    d = D_8010DA50;
    a0 = ret + 16;
    c = 15;
    do {
        *d++ = *a0++;
    } while (--c != -1);
    func_8010C1EC(D_8010DA0C, 0x20);
    func_8010C1EC(D_8010DA90, 0x20);
    return ret;
}
