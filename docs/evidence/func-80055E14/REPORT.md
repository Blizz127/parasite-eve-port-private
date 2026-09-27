# func_80055E14

VRAM `0x80055E14`, size `0x1A0` (104 words), inside `asm/disc1/45F60.s`.

era `-O2 -G8` (existing `era_o2_g8`; `-G0` is 101 words off and `nonzero_pad=6`). `LINK_EXACT`, linked 416 bytes, `nonzero_pad=0`. `leaf_strong_check.py`: `compiled=0x1A0`.

```
TMPDIR=build/tmp python3 tools/analysis/era_link_check.py \
  src/func_80055E14.c 0x80055E14 0x1A0 -O2 -G8
```
