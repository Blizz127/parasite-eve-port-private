extern int func_8007EE84(unsigned char a0, int a1, int a2, int a3);
extern unsigned char func_8007F418(int a0, int a1);

int func_80080D5C(unsigned char a0, int a1, int a2)
{
    int h;
    unsigned char r;

    h = func_8007EE84(a0, a1, 0, 0);
    if (h == 0) {
        return 0;
    }
    do {
        r = func_8007F418(h, a2);
    } while (r == 0);
    return r == 2;
}
