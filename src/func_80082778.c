/* VRAM 0x80082778 / size 0xD4.
 * D_8009B738() yields the record. a1 indexes a 5-byte row at +4; a2-1
 * selects a byte of that row through jtbl_8001206C. Out of range returns 0.
 * A negative a1 returns the row count at +0xE9.
 */
typedef struct Rec82778 Rec82778;
struct Rec82778 {
    unsigned char pad0[4];
    unsigned char *rows;
    unsigned char pad8[0xE9 - 8];
    unsigned char nE9;
};

extern Rec82778 *(*D_8009B738)();

int func_80082778(int a0, int a1, int a2) {
    Rec82778 *p = D_8009B738();
    unsigned char *row;
    int idx;

    if (a1 < 0) {
        return p->nE9;
    }
    if (a1 >= p->nE9) {
        return 0;
    }
    row = p->rows + a1 * 5;
    idx = a2 - 1;
    switch (idx) {
    case 0:
        return row[0];
    case 1:
        return row[1];
    case 2:
        return row[2];
    case 3:
        return row[3];
    case 4:
        return row[4];
    }
    return 0;
}
