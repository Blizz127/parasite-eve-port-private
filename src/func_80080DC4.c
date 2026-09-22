extern int func_8007EE84(int, int, int, int);
extern int func_8007F418(int, int);

int func_80080DC4(int a0, int a1, int a2)
{
    int h;
    int r;

    h = func_8007EE84(a0 & 0xFF, a1, 0, 0);
    if (h == 0) {
        return 0;
    }
    do {
        r = func_8007F418(h, a2) & 0xFF;
    } while (r == 0);
    return r == 2;
}
