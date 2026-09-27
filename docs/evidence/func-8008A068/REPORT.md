# func_8008A068 — LEAF_STRONG=EXACT (lane exeD, 2026-09-27)

- VRAM `0x8008A068`, file `0x7A868`, size `0x2EC` (187 words), `asm/disc1/7A868.s`.
- Semantics: AKAO sequence start. It masks the header words to 24 bits and sets up the
  24-voice records at `D_800B8AC0` (stride `0x11C`).
- Profile `era_o2_g0_cc1_281_unfill_epilogue_delay_slot` (existing):
  `-O2 -G0 -mno-split-addresses`, `ERA_CC1_VER=2.8.1`, `MASPSX_UNFILL_EPILOGUE_DELAY_SLOT=1`.
- YAML: `- [0x7A868, c, func_8008A068]` replaces `[0x7A868, asm]`; the span ends exactly at the
  next row `[0x7AB54, c, func_8008A354]`, so no resume row is needed.

## Verify

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD/disc1.yaml --profiles build/lanes/exeD/disc1_build_profiles.json --deep --only func_8008A068
  -> disc1_preflight: PASS (deep, 2043 c / 218 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD/disc1.yaml --profiles build/lanes/exeD/disc1_build_profiles.json func_8008A068
  -> LEAF_STRONG=EXACT func_8008A068 span=0x2EC vram=0x8008A068 compiled=0x2F0 failures=[none]
```
`profile_necessity.py` skipped: no `--config` option (it reads the main YAML).

## Ladder

The earlier lanes (decompB/tooling2) parked it at 9 words on cc1 2.7.2. Their draft was
`build/archive/fanout/decompB/drafts/func_8008A068.c`, and the residual was an `$a0`/`$a1`
swap between the second header-block pointer and the `-258` mask.

| form | 2.7.2 | 2.8.1 |
|---|---|---|
| archived draft | 9 | 11 |
| + block-scoped `register unsigned char *c3 asm("$4") = D_8009D2C8` for the f8/fC/f0 block | 19 (the `q = a0` prologue copy sinks below the first call) | 2 (epilogue `addiu $sp` in the `jr` slot only) |
| + `MASPSX_UNFILL_EPILOGUE_DELAY_SLOT=1` (patch 15) | 19 | **0** |

This is a 2.8.1-band function (0x80071A84–0x8008D610). The `$a0` pin that broke the 2.7.2
prologue is neutral under 2.8.1. Without patch 15: 2 mismatches.
