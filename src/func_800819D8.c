extern int D_800A3CB4[];
extern unsigned char D_800A3CBC[];
extern int func_80072A54(unsigned char *a0, unsigned char *a1);

int func_800819D8(int a0, unsigned char *a1)
{
    int i;
    int off;
    unsigned char *q;
    int v;

    i = 0;
    q = D_800A3CBC;
    off = 0;
loop:
    v = *(int *)((unsigned char *)D_800A3CB4 + off);
    if (v == 0) {
        goto none;
    }
    if (v == a0) {
        if (func_80072A54(a1, q) == 0) {
            return i + 1;
        }
    }
    q += 0x2C;
    i++;
    off += 0x2C;
    if (i < 0x80) {
        goto loop;
    }
none:
    return -1;
}
