# func_80084168

- VRAM `0x80084168`, size `0x37C` (223 words), file offset `0x74968`
- Profile: `era_o2_g0_cc1_281_fill_branch_macro_split`, flags `-O2 -G0 -mno-split-addresses`, env `ERA_CC1_VER=2.8.1 MASPSX_FILL_BRANCH_MACRO_SPLIT=1`
- Source: `src/func_80084168.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_80084168
  -> disc1_preflight: PASS (deep, 1666 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_80084168
  -> LEAF_STRONG=EXACT
(ERA_CC1_VER=2.8.1 MASPSX_FILL_BRANCH_MACRO_SPLIT=1 python3 tools/analysis/era_link_check.py src/func_80084168.c 0x80084168 0x37C -O2 -G0 -mno-split-addresses)
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

`v = s1[0x36]; flag = (v == 0);` keeps the byte in $v0 (`lbu $v0` / `sltiu $s4,$v0,1`); folding the load into the compare loads straight into $s4 (2-word miss). The two `sw D_8009B79C` stores sit in blez/bgtz slots (patch 10). -fno-schedule-insns from the old draft is NOT load-bearing.
