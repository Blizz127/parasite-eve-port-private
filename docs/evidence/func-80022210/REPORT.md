# func_80022210

- VRAM `0x80022210`, file offset `0x12A10`, size `0x184` (97 words)
- Profile: `era_o2_g8_force_d8009d278_d8009d254_d8009d1a0_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D278,D_8009D254,D_8009D1A0 (new profile)
- Source: `src/func_80022210.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80022210
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80022210
LEAF_STRONG=EXACT func_80022210 span=0x184 vram=0x80022210
```

## Load-bearing levers

`-G8` keeps the byte cursor `D_8009D1D4` (0x464) gp-relative. The `0x200000` flag appears twice, so cc1 CSEs it into callee-saved `$s0` across the calls (the `~0x200000` mask is materialized separately, matching retail). The re-range check `f4 < 3 || f4 >= 0x197 || fE < 4` folds into retail's `lhu; addiu -3; sltiu 0x194`.
