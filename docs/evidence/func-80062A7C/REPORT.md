# func_80062A7C

VRAM `0x80062A7C`, size `0x23C` (143 words), `asm/disc1/5327C.s`.

Looks up the string for `a0` through `func_8005DC4C`, pushes the cursor at `D_8009D12C` when it is below `D_800A22B0`, walks the bytes until `0xFF` while accumulating glyph widths from `func_8005DC28`, then centers that width against `D_8009D138` (`off = (D_8009D138 - (w + 4)) >> 1`) and writes the new `D_8009D124` / `D_8009D128`. It draws with `func_8005EED4` and pops the cursor, including the overflow path `func_800527C0(3)`.

era `-O2 -G8` (existing profile `era_o2_g8`, no environment). `LINK_EXACT`, word mismatches 0, `nonzero_pad=0`. The linked section is 576 bytes (`0x240`) because `.text` is aligned to 16 and `0x23C` is 4 bytes short of that boundary; those bytes are zero. `leaf_strong_check.py`: `LEAF_STRONG=EXACT span=0x23C compiled=0x240`.

```
TMPDIR=build/tmp python3 tools/analysis/era_link_check.py \
  src/func_80062A7C.c 0x80062A7C 0x23C -O2 -G8
```
