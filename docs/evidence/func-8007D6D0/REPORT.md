# func_8007D6D0

- VRAM `0x8007D6D0`, size `0xA8` (42 words), file offset `0x6DED0`
- Profile: `era_o2_g0_cc1_281`, flags `-O2 -G0 -mno-split-addresses`, env `ERA_CC1_VER=2.8.1`
- Source: `src/func_8007D6D0.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_8007D6D0
  -> disc1_preflight: PASS (deep, 1666 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_8007D6D0
  -> LEAF_STRONG=EXACT
(ERA_CC1_VER=2.8.1 python3 tools/analysis/era_link_check.py src/func_8007D6D0.c 0x8007D6D0 0xA8 -O2 -G0 -mno-split-addresses)
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

volatile SPU ctrl (`volatile unsigned short *D_8009B3FC`) and volatile DMA channel words keep stores in source order and out of call delay slots.
