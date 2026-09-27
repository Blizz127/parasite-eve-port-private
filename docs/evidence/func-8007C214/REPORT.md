# func_8007C214

- VRAM `0x8007C214`, size `0x8C` (35 words), file offset `0x6CA14`
- Profile: `era_o2_g0_fill_branch_macro_split`, flags `-O2 -G0`, env `MASPSX_FILL_BRANCH_MACRO_SPLIT=1`
- Source: `src/func_8007C214.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_8007C214
  -> disc1_preflight: PASS (deep, 1666 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_8007C214
  -> LEAF_STRONG=EXACT
(MASPSX_FILL_BRANCH_MACRO_SPLIT=1 python3 tools/analysis/era_link_check.py src/func_8007C214.c 0x8007C214 0x8C -O2 -G0)
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

4-byte struct copy spells the lwl/lwr/swl/swr pair; the absolute `sw D_800BE9E4` lands in the `beqz` slot only with maspsx patch 10 (MASPSX_FILL_BRANCH_MACRO_SPLIT).
