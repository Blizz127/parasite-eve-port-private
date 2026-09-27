# func_80019810

VRAM `0x80019810`, size `0xB4` (45 words). Field-entity command: negate `**a0`, scale `func_80077CF4` / `func_80077DC4` of `D_8009D2F0` halfword `+0x3A` by `<< 4` through `func_8003708C`, and add the products onto `D_8009D2F0` words `+0x28` / `+0x30` through `a0[1]` / `a0[2]`. Returns 1.

`-O2 -G0`. The state pointer is pinned to `$2` and three empty register barriers keep retail's schedule: `lw D_8009D2F0` before the prologue, then `lw` of `a0[0]`, `lh` of `+0x3A`, `lw` of that word, with `negu` in the `jal func_80077CF4` delay slot.

```c
extern unsigned char *D_8009D2F0;
extern int func_80077CF4(int a0);
extern int func_80077DC4(int a0);
extern int func_8003708C(int a0, int a1);

int func_80019810(int **a0) {
    register unsigned char *st asm("$2");
    int *p;
    int idx;
    int neg;
    int t;
    int r;

    st = D_8009D2F0;
    asm volatile("" : "=r"(st) : "0"(st));
    p = a0[0];
    asm volatile("" : "=r"(p) : "0"(p));
    idx = *(short *)(st + 0x3A);
    asm volatile("" : "=r"(idx) : "0"(idx));
    neg = -*p;
    t = func_80077CF4(idx);
    r = func_8003708C(neg, t << 4);
    *a0[1] = *(int *)(D_8009D2F0 + 0x28) + r;
    t = func_80077DC4(*(short *)(D_8009D2F0 + 0x3A));
    r = func_8003708C(neg, t << 4);
    *a0[2] = *(int *)(D_8009D2F0 + 0x30) + r;
    return 1;
}
```

```
python3 tools/analysis/era_link_check.py --all-mismatches src/func_80019810.c 0x80019810 0xB4 -O2 -G0
linked .text 192 bytes, target 0xb4, word mismatches=0, nonzero_pad=0
LINK_EXACT
```
