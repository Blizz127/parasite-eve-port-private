# func_80070FAC

VRAM `0x80070FAC`, size `0x88`. Profile `era_o1_g0_mflo_before_mfhi`:
`-O1 -G0` and `MASPSX_MFLO_BEFORE_MFHI=1`.

Three signed inputs are forced non-negative, squared, added as 64-bit
values, and the sum is shifted right by 20. Each square is a `long long`
held in a union so the low and high halves are the `mflo` and `mfhi`
results. `-O1` still emits `mfhi` before `mflo` and copies each half with
`addu $dst,$tmp,$zero`. The knob retargets those copies (`mflo` then
`mfhi`) and keeps the two hazard `nop`s only in front of the next `mult`.
The third square has no following `mult`, so those `nop`s are dropped and
the add chain follows immediately.

```
TMPDIR=build/tmp MASPSX_MFLO_BEFORE_MFHI=1 \
  python3 tools/analysis/era_link_check.py \
  src/func_80070FAC.c 0x80070FAC 0x88 -O1 -G0
```

`LINK_EXACT`, word mismatches 0, `nonzero_pad=0`. The linked section is 144
bytes; the extra 8 are a zero alignment pad. Without the knob the same
source is 22 words off, starting at the first `mflo`. `leaf_strong_check.py
func_80070FAC` printed `LEAF_STRONG=EXACT` (`compiled=0x90`).
`disc1_preflight.py --deep --only func_80070FAC` passed before the rebuild
(`2034 c / 221 asm / 2 rodata`).
