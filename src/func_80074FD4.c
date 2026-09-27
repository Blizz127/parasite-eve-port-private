/*
 * func_80074FD4 — log via func_80074E28(&D_800118C8, a0), then push a packed
 * 0x80000000 | b3<<16 | b2<<8 | b1 word through D_80095744->f(0x8) with the
 * handler argument D_80095744->v[3] and the constant 8.
 *
 * ROM: ERA_CC1_VER=2.8.1 -O2 -G0 -mno-split-addresses (band compiler; fills the
 * `jr $ra` slot itself — 2.7.2 needs patch 3 for the same bytes). The three
 * byte parameters must be declared `unsigned char`: int params with `& 0xFF`
 * masks move the `la $a0,D_800118C8` later in the prologue (9-16 words).
 */
extern unsigned int *D_80095744;
extern char D_800118C8;
extern void func_80074E28(char *a0, int a1);
void func_80074FD4(int a0, unsigned char a1, unsigned char a2, unsigned char a3) {
    unsigned int *v1;
    func_80074E28(&D_800118C8, a0);
    v1 = D_80095744;
    (*(void (**)(int, int, int, int))(v1 + 2))(v1[3], a0, 8, (a3 << 16) | 0x80000000 | (a2 << 8) | a1);
}
