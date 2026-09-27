# func_80015AF0

- VRAM `0x80015AF0`, file offset `0x62F0`, size `0xBC` (47 words)
- Profile: `era_o2_g8_force_d8009d1a0_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D1A0 (via profile)
- Source: `src/func_80015AF0.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80015AF0
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80015AF0
LEAF_STRONG=EXACT func_80015AF0 span=0xBC vram=0x80015AF0
```

## Load-bearing levers

Same shape as func_80015BAC: `-G8` + `D_8009D1A0` forced absolute; `D_800B0CD8` as an unsized array accessed directly as `D_800B0CD8[0]`.
