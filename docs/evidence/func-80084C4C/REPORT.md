# func_80084C4C — LEAF_STRONG=EXACT (lane exeD, 2026-09-27)

- VRAM `0x80084C4C`, file `0x7544C`, size `0x264` (153 words), `asm/disc1/7544C.s`. libpad
  per-port state step: copies the receive buffer, runs the D_8009B728 notify callback, then a
  switch on the port state byte at +0x46.
- Profile `era_o2_g0_cc1_281_drop_target_dup_li` (new): `-O2 -G0 -mno-split-addresses`,
  `ERA_CC1_VER=2.8.1`, `MASPSX_DROP_TARGET_DUP_LI=1` (**new maspsx LOCAL PATCH 35**, written by lane exeD).
- YAML: `- [0x7544C, c, func_80084C4C]` replaces `[0x7544C, asm]`; the span ends at the existing
  `[0x756B0, c, func_80084EB0]`.

## Verify

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD/disc1.yaml --profiles build/lanes/exeD/disc1_build_profiles.json --deep --only func_80084C4C
  -> disc1_preflight: PASS (deep, 2046 c / 216 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD/disc1.yaml --profiles build/lanes/exeD/disc1_build_profiles.json func_80084C4C
  -> LEAF_STRONG=EXACT func_80084C4C span=0x264 vram=0x80084C4C compiled=0x270 failures=[none]
```

## Ladder (diff lines = aligned objdump diff)

| form | result |
|---|---|
| archived draft (2.8.1) | 21 diff lines |
| switch with the shared increment label placed in case 1 (case 0 does `goto bump`, matching retail's layout) | 15 |
| `int old` (not `unsigned char`), so the saved type byte is not re-`andi`'d | 10 |
| `register int r asm("$2")` for the callback result: the tail becomes `lbu v1 / addu v1,v1,v0 / sb v1`, and cc1 stops cross-jumping it with the case `sb v0,70(s0)` stores | 1 diff line (27 mismatches from a 1-word shift) |
| + patch 35 | **0** |

The last word: cc1 2.8.1 fills the `beq v1,v0(0xFE),Lfe` slot with the fall-through's `li v0,255`
and keeps the case-0xFE body's own `li v0,255`. Retail's reorg stole the target's copy, so the
0xFE body is only `j end / sb v0,0x46(s0)`. Case order and flags (-fno-thread-jumps,
-fno-cse-*, -fno-rerun-cse-after-loop, -fno-schedule-insns, -fno-expensive-optimizations) do not
change it.

Patch 35 (`MASPSX_DROP_TARGET_DUP_LI`) deletes a branch target's leading `li $r,K` when:
- (H1) the label's only reference is a noreorder conditional branch whose slot is the same
  `li $r,K`;
- (H2) nothing falls through into the label;
- (H3) `$r` is neither `$0` nor `$at`.

Tests: `tools/era/maspsx/tests/test_drop_target_dup_li.py` (6), registered in setup_era.sh
MASPSX_TRACKED and the `.gitignore` negations. Sweep:
`maspsx_knob_sweep.py --knob MASPSX_DROP_TARGET_DUP_LI --baseline <patch-34 copy>` gave
`4550 era leaves, {'IDENT': 4550}`, `flag-off non-identical: none`, `knob-on changes (0)`.
The maspsx suite gives 468 OK.
