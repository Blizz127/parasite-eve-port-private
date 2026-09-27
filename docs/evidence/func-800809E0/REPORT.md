# func_800809E0

- VRAM `0x800809E0`, size `0xDC` (55 words), file offset `0x711E0`
- Profile: `era_o2_g0_cc1_281_fill_call_delay_slot_branch_macro_split`, flags `-O2 -G0 -mno-split-addresses`, env `ERA_CC1_VER=2.8.1 MASPSX_FILL_CALL_DELAY_SLOT=1 MASPSX_FILL_BRANCH_MACRO_SPLIT=1`
- Source: `src/func_800809E0.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_800809E0
  -> disc1_preflight: PASS (deep, 1666 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_800809E0
  -> LEAF_STRONG=EXACT
(ERA_CC1_VER=2.8.1 MASPSX_FILL_CALL_DELAY_SLOT=1 MASPSX_FILL_BRANCH_MACRO_SPLIT=1 python3 tools/analysis/era_link_check.py src/func_800809E0.c 0x800809E0 0xDC -O2 -G0 -mno-split-addresses)
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

gcc 2.8.1 band leaf; absolute store / `la` macro seconds go into jal and conditional-branch slots (maspsx patches 8 + 10).
