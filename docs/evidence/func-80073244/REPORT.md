# func_80073244

VRAM `0x80073244`, size `0x50` (20 words), `asm/disc1/63760.s`. No calls.

Unsigned 64-bit compare of `(a.high, a.low)` against `(b.high, b.low)`. The two halves arrive in `$a1:$a0` and `$a3:$a2`. Returns 1 when the first value is greater, -1 when it is smaller, and 0 when they are equal. All four arguments are stored to `0($sp)` through `12($sp)` before the compares; the `$a3` store fills the first `bnez` delay. A `UD { unsigned low, high; }` argument pair is what emits those homes.

era `-O2 -G0` with `MASPSX_SLTU_A0_LT_A2_DEST_V1=1`. Profile `era_o2_g0_sltu_a0_lt_a2_dest_v1`. `LINK_EXACT`, word mismatches 0, `nonzero_pad=0`, linked `.text` 80 bytes equals `0x50`. `leaf_strong_check.py`: `LEAF_STRONG=EXACT span=0x50 compiled=0x50`. `disc1_preflight.py --deep --only func_80073244` passed before the rebuild.

The low-half test `a.low < b.low` is `sltu $v1,$a0,$a2` / `bnez $v1` in retail. cc1 reuses `$v0`, which still holds the `li $v0,1` from the previous delay. The knob rewrites only that pair, and only when it sits after `bne $v0,$0` / `li $v0,1` and before `li $v0,-1`. Without the knob the leaf is 2 words off (`0086102b` / `14400004` instead of `0086182b` / `14600004`).

```
TMPDIR=build/tmp MASPSX_SLTU_A0_LT_A2_DEST_V1=1 \
python3 tools/analysis/era_link_check.py \
  src/func_80073244.c 0x80073244 0x50 -O2 -G0
```
