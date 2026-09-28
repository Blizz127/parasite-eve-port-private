extern int D_8009B578[];
extern int D_8009B554;
extern void (*volatile D_800A36A4)(int, int);
extern void (*volatile D_800A36A8)(int, int);
extern void (*volatile D_800A36AC)(int, int);

#define FLG (((unsigned char *)st)[-0xC])

void func_80080404(unsigned char a0, int a1)
{
    int *st = D_8009B578;
    int s, t;

    if (st[0] == 12) {
        st[0] = 13;
    }
    if (a0 == 2) {
        s = st[0];
        if (s == 13) {
            if (!(FLG & 0x10)) {
                st[0] = 14;
                st[-1] = 2;
                st[1] = 21;
                st[6]++;
            }
        } else if (s == 14) {
            t = st[1];
            if (t == 21) {
                if (!(FLG & 0x10)) {
                    st[-1] = 2;
                    st[0] = 14;
                    st[1] = 22;
                    st[5] = 0;
                }
            } else if (t == 22) {
                if (FLG & 2) {
                    st[-1] = 2;
                    st[0] = 14;
                    st[1] = 23;
                } else if (st[5] > 300) {
                    st[-1] = 3;
                    if (D_800A36AC != 0) {
                        D_800A36AC(5, a1);
                    }
                }
            } else if (t == 23) {
                st[-1] = 2;
                st[0] = 14;
                st[1] = 24;
            } else if (t == 24) {
                if (FLG == 2) {
                    st[-1] = 1;
                    st[0] = 11;
                    st[1] = 0;
                    if (D_800A36AC != 0) {
                        D_800A36AC(2, a1);
                    }
                }
            }
        } else if ((unsigned int)(s - 16) < 2) {
            if (st[10] == 0 && !(FLG & 2)) {
                st[-1] = 1;
                st[0] = 11;
                if (D_800A36A8 != 0 && st[-9] != 0) {
                    D_800A36A8(5, a1);
                }
                if (D_800A36A4 != 0 && D_8009B554 != 0) {
                    D_800A36A4(5, a1);
                }
            }
        }
    } else if (FLG & 0x10) {
        st[-1] = 2;
        st[0] = 12;
    } else if ((unsigned int)(st[0] - 16) < 2) {
        st[-1] = 1;
        st[0] = 11;
    }
}
