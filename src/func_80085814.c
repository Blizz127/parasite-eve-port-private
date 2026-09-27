/*
 * func_80085814 - SPU voice attribute setter (3 voices, 16-byte records at
 * *D_8009B7D0).  vram 0x80085814, size 0x9C.  era -O2 -G0 (default).
 *
 * Levers: the record pointer is pointer-to-volatile, which keeps the two
 * leading `sh` stores out of the `beqz` delay slot (retail fills it with the
 * next `andi` instead); `i == 0 || i == 1` folds to retail's `sltiu 2`.
 */
typedef struct {
    short h0;
    short h2;
    short h4;
    short h6;
    short h8;
    short pad[3];
} Voice85814;

extern volatile Voice85814 *D_8009B7D0;

int func_80085814(unsigned short a0, short a1, int a2)
{
    int i = a0;
    int m = 0x48;

    if (i >= 3) {
        return 0;
    }
    D_8009B7D0[i].h4 = 0;
    D_8009B7D0[i].h8 = a1;
    if (i == 0 || i == 1) {
        if (a2 & 0x10) {
            m = 0x49;
        }
        if (!(a2 & 1)) {
            m |= 0x100;
        }
    } else if (i == 2) {
        if (!(a2 & 1)) {
            m = 0x248;
        }
    }
    if (a2 & 0x1000) {
        m |= 0x10;
    }
    D_8009B7D0[i].h4 = m;
    return 1;
}
