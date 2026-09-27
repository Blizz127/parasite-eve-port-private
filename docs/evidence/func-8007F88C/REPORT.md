# func_8007F88C

- VRAM `0x8007F88C`, size `0xD4` (53 words), file offset `0x7008C`
- Profile: `era_o2_g0_cc1_281`, flags `-O2 -G0 -mno-split-addresses`, env `ERA_CC1_VER=2.8.1`
- Source: `src/func_8007F88C.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_8007F88C
  -> disc1_preflight: PASS (deep, 1666 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_8007F88C
  -> LEAF_STRONG=EXACT
(ERA_CC1_VER=2.8.1 python3 tools/analysis/era_link_check.py src/func_8007F88C.c 0x8007F88C 0xD4 -O2 -G0 -mno-split-addresses)
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

`unsigned char a0` parameter reproduces the per-use `andi 0xFF`; switch {1,5 -> D_800A3520; 4 -> D_800A3530} with a `goto tail` default.
