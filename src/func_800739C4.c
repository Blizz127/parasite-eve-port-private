/*
 * func_800739C4 — event deliver: record (a0, a1) in D_80094564/D_80094568,
 * then post func_80073A34(0xF4000002, 0x301/0x302) for event codes 0x21/0x22.
 *
 * ROM: era gcc-2.7.2-psx -O2 -G0 + MASPSX_FILL_BRANCH_MACRO_SPLIT=1 (maspsx
 * patch 10: the second `sw` of the D_80094568 `$at` store macro lands in the
 * `beq` delay slot). Without the knob: 2 words differ.
 */
extern int D_80094564;
extern int D_80094568;
void func_80073A34(unsigned int a, int b);

int func_800739C4(int a0, int a1) {
    D_80094564 = a0;
    D_80094568 = a1;
    switch (a0) {
    case 0x21:
        func_80073A34(0xF4000002, 0x301);
        break;
    case 0x22:
        func_80073A34(0xF4000002, 0x302);
        break;
    }
    return 0;
}
