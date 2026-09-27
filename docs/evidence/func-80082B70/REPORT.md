# func_80082B70

- VRAM `0x80082B70`, size `0x16C` (91 words), file offset `0x73370`
- Profile: `era_o2_g0_fill_branch_macro_split`, flags `-O2 -G0`, env `MASPSX_FILL_BRANCH_MACRO_SPLIT=1`
- Source: `src/func_80082B70.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_80082B70
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_80082B70
  -> LEAF_STRONG=EXACT func_80082B70 span=0x16C failures=[none]
(MASPSX_FILL_BRANCH_MACRO_SPLIT=1 python3 tools/analysis/era_link_check.py src/func_80082B70.c 0x80082B70 0x16C -O2 -G0)
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

Timeout counters written as 'p = &D_800A5AC0; v = *p; if (v < 0x96) *p = v + 1;' (the value local puts the pointer in $a0 and the value in $v1 as retail; '*p = *p + 1' swaps them); the D_8009B78C/D_8009B764 absolute stores sit in beqz/jal slots via maspsx patch 10 (85 words without). 2.7.2.
