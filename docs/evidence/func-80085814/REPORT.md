# func_80085814

- VRAM `0x80085814`, size `0x9C` (39 words), file offset `0x76014`
- Profile: `era_o2_g0`, flags `-O2 -G0`
- Source: `src/func_80085814.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_80085814
  -> disc1_preflight: PASS (deep, 1666 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_80085814
  -> LEAF_STRONG=EXACT
python3 tools/analysis/era_link_check.py src/func_80085814.c 0x80085814 0x9C -O2 -G0
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

SPU voice records typed pointer-to-volatile so the two leading `sh` stores stay out of the beqz delay slot (retail fills it with the next andi); `i == 0 || i == 1` folds to `sltiu 2`.
