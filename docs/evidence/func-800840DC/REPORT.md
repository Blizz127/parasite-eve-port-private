# func_800840DC

- VRAM `0x800840DC`, size `0x8C` (35 words), file offset `0x748DC`
- Profile: `era_o2_g0_cc1_281`, flags `-O2 -G0 -mno-split-addresses`, env `ERA_CC1_VER=2.8.1`
- Source: `src/func_800840DC.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_800840DC
  -> disc1_preflight: PASS (deep, 1666 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_800840DC
  -> LEAF_STRONG=EXACT
(ERA_CC1_VER=2.8.1 python3 tools/analysis/era_link_check.py src/func_800840DC.c 0x800840DC 0x8C -O2 -G0 -mno-split-addresses)
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

same `v = a0[0x36]; flag = (v == 0);` spelling as func_80084168; `if (r == 0x5A || r == 0 || r < 0) return r; return -9;`.
