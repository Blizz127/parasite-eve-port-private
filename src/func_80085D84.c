extern int D_8009B438;
void func_80085DC4();

int func_80085D84(int a0) {
    int old = D_8009B438;
    if (a0 != old) {
        D_8009B438 = a0;
        func_80085DC4();
    }
    return old;
}
