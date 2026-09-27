/*
 * func_80074F44 — log via func_80074E28(&D_800118BC, a0), then push the packed
 * b3<<16 | b2<<8 | b1 word through D_80095744->f(0x8)(v[3], a0, 8, word).
 *
 * ROM: ERA_CC1_VER=2.8.1 -O2 -G0 -mno-split-addresses. The load-bearing
 * lever that closes the old prologue-interleave PARK is declaring the three
 * byte parameters `unsigned char` (the andi 0xFF are the incoming-arg
 * truncations), which moves `la $a0,D_800118BC` up beside `move $s3,$a0`.
 */
extern unsigned int *D_80095744;
extern char D_800118BC;
extern void func_80074E28(char *a0, int a1);
void func_80074F44(int a0, unsigned char a1, unsigned char a2, unsigned char a3) {
    unsigned int *v0;
    func_80074E28(&D_800118BC, a0);
    v0 = D_80095744;
    (*(void (**)(int, int, int, int))(v0 + 2))(v0[3], a0, 8, (a3 << 16) | (a2 << 8) | a1);
}
