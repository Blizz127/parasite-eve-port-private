# func_800677FC

VRAM `0x800677FC`, size `0x1C8` (114 words), carved from `asm/disc1/57FFC.s`.

Publishes the 52-byte view record selected by `D_800BCFFD` (via `func_80066800`), stores the signed half-average of bytes `+0x2C/+0x2E` and the logical half-average of `+0x30/+0x32`, copies those four halfwords into `D_800BCFAC`..`D_800BCFB2`, recentres the container halfwords at `+0x38/+0x3A`, then walks `*(unsigned short *)(base+6)` records of stride `0x38` through `func_80066F60`. A nonzero return from that call returns `-0x12`. The fall-through clears `D_800BCFFA`/`D_800BCFFB` and `D_800BCF88 &= ~0xC00`.

era `-O2 -G0` plus `MASPSX_FRAME48_S5_AFTER_S2=1` (patch 32). The knob is load-bearing: the same source at `-O2 -G0` with the knob off is 17 words off (the `$s5` save is emitted before the `$s2=$a0` pair, and the frame is 48 rather than 56). With the knob, `era_link_check.py` prints `LINK_EXACT`, linked 464 bytes, `nonzero_pad=0` (one trailing zero word; `leaf_strong_check.py` reports `compiled=0x1D0`).

```
TMPDIR=build/tmp MASPSX_FRAME48_S5_AFTER_S2=1 \
  python3 tools/analysis/era_link_check.py \
  src/func_800677FC.c 0x800677FC 0x1C8 -O2 -G0
```
