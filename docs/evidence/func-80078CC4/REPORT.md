# func_80078CC4

VRAM `0x80078CC4`, size `0x138` (78 words), `asm/disc1/694B8.s`. No calls.

Scales five packed halfword pairs in `p` by three unsigned weights in `m`. Each low half is sign-extended, multiplied, shifted right 12, and masked to 16 bits. The high half is an arithmetic `>> 16`, multiplied, shifted right 12, and placed in the top half. The fifth word stores only the shifted low product, with no mask. Returns `p`.

era `-O2 -G0` with `MASPSX_MULT_MFLO_TO_MULTU=1`. Profile `era_o2_g0_mult_mflo_to_multu`. `LINK_EXACT`, word mismatches 0, `nonzero_pad=0`. The linked section is 320 bytes (`0x140`) because `.text` is aligned to 16 and `0x138` is 8 bytes short of that boundary; those bytes are zero. `leaf_strong_check.py`: `LEAF_STRONG=EXACT span=0x138 compiled=0x140`. `disc1_preflight.py --deep --only func_80078CC4` passed before the rebuild.

cc1 emits `mult` whenever only `mflo` is consumed, because those low 32 bits match `multu`. Retail writes `multu` at all nine products. The knob rewrites `mult $a,$b` only when the next real instruction is `mflo`. A following `mfhi` stays `mult`. Without the knob the leaf is those 9 words. The matrix weights are pinned to `$t3/$t4/$t5` and the packed word to `$t0` so the address temps match retail. Empty `asm` barriers keep the word live across `>> 16` (`sra $t2,$t0,16`) and keep each store ahead of the next load.

```
TMPDIR=build/tmp MASPSX_MULT_MFLO_TO_MULTU=1 \
python3 tools/analysis/era_link_check.py \
  src/func_80078CC4.c 0x80078CC4 0x138 -O2 -G0
```
