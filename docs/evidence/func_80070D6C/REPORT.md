# func_80070D6C — MATCHED (25 words, LINK_EXACT)

VRAM `0x80070D6C`, size `0x64`. Lagged-Fibonacci RNG advance.

```c
unsigned int func_80070D6C(void) {
    register unsigned char *base asm("$8");
    register int *i1 asm("$15");
    register int *i2 asm("$24");
    register int t1 asm("$9");
    register int t2 asm("$10");
    register unsigned int *p asm("$11");
    register unsigned int *q asm("$12");
    register unsigned int s asm("$13");
    register unsigned int u asm("$14");
    register unsigned int z asm("$0");
    register unsigned int ret asm("$2");

    base = (unsigned char *)0x80070E0Cu;
    i1 = (int *)0x80070E04u;
    i2 = (int *)0x80070E08u;
    t1 = *i1;
    t2 = *i2;
    asm volatile("" : "=r"(base) : "0"(base));
    asm volatile("" : "=r"(t1) : "0"(t1));
    asm volatile("" : "=r"(t2) : "0"(t2));
    p = (unsigned int *)(base + t1);
    q = (unsigned int *)(base + t2);
    asm volatile("" : "=r"(p) : "0"(p));
    asm volatile("" : "=r"(q) : "0"(q));
    s = *p;
    u = *q;
    s = s + u;
    *p = s;
    ret = z | s;
    t1 = t1 - 4;
    t2 = t2 - 4;
    if (t1 < 0) {
        t1 = (int)(z | 0x40u);
    }
    *i1 = t1;
    if (t2 < 0) {
        t2 = t2 | 0x40;
    }
    *i2 = t2;
    return ret;
}
```

Flags: `-O1 -G0`. No maspsx env. The `$0` or produces retail `or $v0,$zero,$t5` and `ori $t1,$zero,0x40` (plain `li` is `addiu`). Integer address constants are `lui`/`ori`, not `la`'s `addiu`.

Gate: `python3 tools/analysis/era_link_check.py --all-mismatches src/func_80070D6C.c 0x80070D6C 0x64 -O1 -G0`

```
linked .text 112 bytes, target 0x64, word mismatches=0, nonzero_pad=0
LINK_EXACT
```
