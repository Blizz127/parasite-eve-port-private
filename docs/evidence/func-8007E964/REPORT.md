# func_8007E964

VRAM `0x8007E964`, size `0x224` (137 words), `asm/disc1/6F164.s`.

CD completion for the head entry of `D_800A3540`. A nonzero command is copied into `D_800A3510` and `func_80080998` copies the result bytes. Status 2 either completes the command (`func_8007EB88`, the entry callback, `func_8007E5C4`) or advances the head. Status 5 retries while `retry > 0` or `retry == -1`, and the not-equal arm stores `retry - 1`. The tail calls `D_800B8AB0` when set, then `func_8007FB44` when `func_8007FBF0(0) == 1` and `D_800A3608 > 0`.

cc1 2.8.1, `-O2 -G0 -mno-split-addresses`, `MASPSX_ADDU_NEG1_BEQ_DELAY=1` (profile `era_o2_g0_cc1_281_addu_neg1_beq_delay`). Without the knob the only mismatch is `0x8007EA94`: retail `addiu $v0,$v1,-1`, cc1 `addu $v0,$v1,$v0` because the compare's `li $v0,-1` is still live. cc1 2.7.2 is 113 words off. Dropping `-mno-split-addresses` is 66 words off.

`LINK_EXACT`, word mismatches 0, `nonzero_pad=0`. The linked section is 560 bytes (`0x230`) because `.text` is aligned to 16 and `0x224` is 12 bytes short of that boundary; those bytes are zero. `leaf_strong_check.py`: `LEAF_STRONG=EXACT span=0x224 compiled=0x230`.

```
TMPDIR=build/tmp ERA_CC1_VER=2.8.1 MASPSX_ADDU_NEG1_BEQ_DELAY=1 \
  python3 tools/analysis/era_link_check.py \
  src/func_8007E964.c 0x8007E964 0x224 -O2 -G0 -mno-split-addresses
```
