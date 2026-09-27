# func_800148E0

- VRAM `0x800148E0`, file offset `0x50E0`, size `0x2C0` (176 words)
- Profile: `era_o2_g8_force_d8009d2f0_d8009d254_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2F0,D_8009D254 (new profile)
- Source: `src/func_800148E0.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_800148E0
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_800148E0
LEAF_STRONG=EXACT func_800148E0 span=0x2C0 vram=0x800148E0
```

## Load-bearing levers

Crashed-session draft verified unchanged: relative-delta variant of the func_800136C0 camera walk.
