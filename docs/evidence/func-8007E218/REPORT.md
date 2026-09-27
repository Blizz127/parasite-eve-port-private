# func_8007E218 — LINK_EXACT

VRAM `0x8007E218`, size `0x14` (5 words). Same tail-jump shape as `func_8007E204`, through `jtbl_800A34CC`.

```c
extern void (*jtbl_800A34CC)();

void func_8007E218(void) {
    register void (*f)() asm("$9");
    f = jtbl_800A34CC;
    goto *(void *)f;
}
```

Flags `-O2 -G0`. No extra env.

`python3 tools/analysis/era_link_check.py --all-mismatches src/func_8007E218.c 0x8007E218 0x14 -O2 -G0`

```
linked .text 32 bytes, target 0x14, word mismatches=0, nonzero_pad=0
LINK_EXACT
```
