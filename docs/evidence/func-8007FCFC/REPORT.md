# func_8007FCFC

- VRAM `0x8007FCFC`, size `0x128` (74 words), file offset `0x704FC`
- Profile: `era_o2_g0_cc1_281`, flags `-O2 -G0 -mno-split-addresses`, env `ERA_CC1_VER=2.8.1`
- Source: `src/func_8007FCFC.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_8007FCFC
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_8007FCFC
  -> LEAF_STRONG=EXACT func_8007FCFC span=0x128 failures=[none]
(ERA_CC1_VER=2.8.1 python3 tools/analysis/era_link_check.py src/func_8007FCFC.c 0x8007FCFC 0x128 -O2 -G0 -mno-split-addresses)
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

Two distinct base symbols as retail uses them (D_8009B558 for the request record, D_8009B598 = record+0x40 for the timeout block with negative offsets); a fresh pointer local t for the final issue so it lands in $s0 (the reused a0 register) rather than $s1; the failure path is written first ('!= 0' clears +0x40/+0x44 and returns 0) to reproduce retail's bnez polarity.
