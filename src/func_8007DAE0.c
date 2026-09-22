extern unsigned short *D_8009B3FC;
extern int D_8009B424;

void func_8007DAE0(int a0, unsigned int a1, int a2)
{
    if (a2 == 0) {
        D_8009B3FC[a0] = a1;
    } else {
        D_8009B3FC[a0] = a1 >> D_8009B424;
    }
}
