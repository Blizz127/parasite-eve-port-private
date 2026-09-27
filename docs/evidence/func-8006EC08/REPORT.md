# func_8006EC08

VRAM `0x8006EC08`, size `0x64` (25 words), `asm/disc1/5F408.s`. No calls.

Reads signed byte `D_800B0DBA`. Returns 0 when it is 0. Otherwise shifts either `-1` or unsigned half `D_800B0DBC` by 16. A non-positive result returns 0. Signed byte `D_800B0DBB` then selects 2 or 1. An 8-byte frame is allocated and freed around the body; nothing is stored there.

era `-O2 -G0` with `MASPSX_DUP_HALF_SHIFT=1`. Profile `era_o2_g0_dup_half_shift`. `LINK_EXACT`, word mismatches 0, `nonzero_pad=0`. The linked section is 112 bytes (`0x70`) because `.text` is aligned to 16 and `0x64` is 12 bytes short of that boundary; those bytes are zero. `leaf_strong_check.py`: `LEAF_STRONG=EXACT span=0x64 compiled=0x70`. `disc1_preflight.py --deep --only func_8006EC08` passed before the rebuild.

cc1 tests the copied byte with `sll $v0,$v0,24` / `beq $v0` and folds `-1 << 16` to `li $v0,-65536`. Retail tests `$v1`, writes `addiu $v0,$zero,-1` in that delay, and executes `sll $v0,$v0,16` on both arms (the halfword arm in a `j` delay). The knob rewrites only that window. Without it the leaf is 6 words off.

```
TMPDIR=build/tmp MASPSX_DUP_HALF_SHIFT=1 \
python3 tools/analysis/era_link_check.py \
  src/func_8006EC08.c 0x8006EC08 0x64 -O2 -G0
```
