# func_8003708C

VRAM `0x8003708C`, size `0x1C`, file `asm/disc1/2788C.s` before the carve.
Profile `era_o2_g0_mflo_before_mfhi`: `-O2 -G0` and
`MASPSX_MFLO_BEFORE_MFHI=1`.

Signed `(a0 * a1)` with the low half shifted right 16 and the high half
shifted left 16, then OR'd. The halves come from a little-endian union so
cc1 emits `mult` (not `multu`) and a `mfhi` / `mflo` pair. Those reads land
in `$a1` / `$a0`; the shifts copy them into `$v0` / `$v1`. The knob folds
that copy:

```
mult $a0,$a1
mflo $v0
mfhi $v1
srl  $v0,$v0,16
sll  $v1,$v1,16
jr   $ra
or   $v0,$v1,$v0
```

The earlier park (`PARK.md`) stopped at five words: `mfhi` before `mflo`,
and an `unsigned` multiply that selected `multu`. Both are closed here.
`-O1 -G0` with the same knob is also `LINK_EXACT`; the integrated profile
is `-O2 -G0` because that is the default optimization and the knob is the
only load-bearing delta.

```
TMPDIR=build/tmp MASPSX_MFLO_BEFORE_MFHI=1 \
  python3 tools/analysis/era_link_check.py \
  src/func_8003708C.c 0x8003708C 0x1C -O2 -G0
```

`LINK_EXACT`, word mismatches 0, `nonzero_pad=0`. The linked section is 32
bytes; the extra 4 are a zero alignment pad. Without the knob the same
source is 4 words off, at the `mflo` / `mfhi` pair and both shifts.
`leaf_strong_check.py func_8003708C` printed `LEAF_STRONG=EXACT`
(`compiled=0x20`). `disc1_preflight.py --deep --only func_8003708C` passed
(`2035 c / 220 asm / 2 rodata`). The disc-1 exact rebuild then passed:
`spans=[2035 c, 220 asm, 2 rodata]`, both SHA-1s
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`, `VERIFY_SWEEP=PASS leaves=2035`.
Route coverage moved with this leaf (`funcs=1261/1453`, `c_words=68845`).
