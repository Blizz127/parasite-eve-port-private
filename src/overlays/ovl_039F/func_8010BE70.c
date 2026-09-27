extern int D_8010DA10[];
extern int D_8010DA50[];
extern int D_8010DA94[];

int *func_8010BE70(int *a0)
{
    int *d;
    int *s;
    int c;

    d = a0;
    s = D_8010DA10;
    c = 15;
    do {
        *d++ = *s++;
    } while (--c != -1);
    d = a0 + 16;
    s = D_8010DA50;
    c = 15;
    do {
        *d++ = *s++;
    } while (--c != -1);
    d = a0 + 32;
    s = D_8010DA94;
    c = 31;
    do {
        *d++ = *s++;
    } while (--c != -1);
    return a0;
}
