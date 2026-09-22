extern void func_80081D74(void *a0, int a1);
extern void func_80082444();
extern int D_800B28F8;

void func_80082400(unsigned char a0)
{
    if (a0 == 2) {
        func_80081D74(func_80082444, 0);
    } else {
        D_800B28F8 = 2;
    }
}
