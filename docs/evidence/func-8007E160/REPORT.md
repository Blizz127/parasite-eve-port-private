# func_8007E160

VRAM `0x8007E160`, size `0x40` (16 words), `asm/disc1/6E960.s`. No calls.

Loads `D_8009B4B4`. Returns 0 when bit 0 of word `+4` is clear. Otherwise returns 1 when bit 0 of word `+0` is set, and 0 when it is clear. Both returns share one `jr $ra`. The early 0 is `addu $v0,$zero,$zero` in the first `beqz` delay. The 1 is `li $v0,1` in the `bnez` delay. The fallthrough zeroes `$v0` again and joins that `jr`.

era `-O2 -G0` with `MASPSX_DROP_JUMP_TO_EPILOGUE=1`. Profile `era_o2_g0_drop_jump_to_epilogue`. `LINK_EXACT`, word mismatches 0, `nonzero_pad=0`, linked `.text` 64 bytes equals `0x40`. `leaf_strong_check.py`: `LEAF_STRONG=EXACT span=0x40 compiled=0x40`. `disc1_preflight.py --deep --only func_8007E160` passed before the rebuild.

cc1 folds `if (bit) return 1; return 0` into `return bit`. Assigning `1` to a local and pinning it with an empty `asm` keeps the `li` / `bnez` shape, but cc1 also emits `j $Lepi` / `addu $v0,$zero,$zero` to step over that empty asm, and `$Lepi` is the next `j $31`. The knob deletes that jump. The zeroing delay stays as the fallthrough into the epilogue. Without the knob the leaf is 5 words off and the section grows to 80 bytes.

```
TMPDIR=build/tmp MASPSX_DROP_JUMP_TO_EPILOGUE=1 \
python3 tools/analysis/era_link_check.py \
  src/func_8007E160.c 0x8007E160 0x40 -O2 -G0
```
