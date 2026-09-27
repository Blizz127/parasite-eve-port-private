# func_8007E0F8

- VRAM `0x8007E0F8`, size `0x68` (26 words), file offset `0x6E8F8`
- Profile: `era_o2_g0`, flags `-O2 -G0`
- Source: `src/func_8007E0F8.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_8007E0F8
  -> disc1_preflight: PASS (deep, 1666 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_8007E0F8
  -> LEAF_STRONG=EXACT
python3 tools/analysis/era_link_check.py src/func_8007E0F8.c 0x8007E0F8 0x68 -O2 -G0
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

`volatile int c[3]` countdown (a scalar gives an 8-byte frame; retail reserves 0x10); `while (--c[0] != -1)` gives the rotated pre-test/post-test pair with `return 0` in both slots.
