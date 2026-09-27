# func_8008A92C

VRAM `0x8008A92C`, size `0x1F0` (124 words), `asm/disc1/7B12C.s`.

If both arguments are zero the function returns. Otherwise a nonzero word at `a0+8` is passed as the second argument of `func_8008A400(0, word)`. The scan reloads `D_800BCC34` and `D_800BCD50` with bit `0x800000`. When both arguments are nonzero it walks pairs (`bit | (bit << 1)`) from `0x400000` for 11 steps of `-0x11C`; otherwise it walks single bits for 12 steps. Exhausting the count calls `func_8008A400(0, 0x40000000)` and, if `D_800BCD50` is unchanged, replaces the mask with `0x80000000` and restarts while that mask is non-negative. A negative mask returns. Otherwise `func_8008A750` / `func_8008A8CC` run for each nonzero argument, the second record is ORed with `0x10000` at `+0x38` when both arguments were nonzero, then `D_8009D2C4 |= 0x10` and `func_80089960` / `func_80089B28` / `func_80089CF0`.

era `-O2 -G0` (default profile, no assignment). `LINK_EXACT`, linked 496 bytes equals `0x1F0`, `nonzero_pad=0`. `leaf_strong_check.py`: `LEAF_STRONG=EXACT span=0x1F0 compiled=0x1F0`.

```
TMPDIR=build/tmp python3 tools/analysis/era_link_check.py \
  src/func_8008A92C.c 0x8008A92C 0x1F0 -O2 -G0
```
