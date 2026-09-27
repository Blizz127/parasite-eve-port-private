# func_8003CCB0

VRAM `0x8003CCB0`, size `0x248` (146 words), `asm/disc1/2D4B0.s`. No calls.

era `-O2 -G0` (default profile, no assignment). `LINK_EXACT`, linked 592 bytes, `nonzero_pad=0` (one trailing zero word). `leaf_strong_check.py`: `compiled=0x250`, `span=0x248`.

```
TMPDIR=build/tmp python3 tools/analysis/era_link_check.py \
  src/func_8003CCB0.c 0x8003CCB0 0x248 -O2 -G0
```
