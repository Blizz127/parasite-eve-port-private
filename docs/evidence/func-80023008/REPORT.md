# func_80023008

- VRAM `0x80023008`, file offset `0x13808`, size `0x124` (73 words)
- Profile: `era_o2_g8_force_d8009d278_d8009d254_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D278,D_8009D254 (via profile)
- Source: `src/func_80023008.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80023008
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80023008
LEAF_STRONG=EXACT func_80023008 span=0x124 vram=0x80023008
```

## Load-bearing levers

`-G8` keeps `D_8009D294` (0x524) / `D_8009D200` (0x490) gp-relative while `D_8009D278`/`D_8009D254` are forced absolute; plain if / else-if / else chain (cc1 shares the `func_8006DE80` call tail between the first and last arms); the 10-bit countdown is `(fC & ~0x3FF) | (((fC & 0x3FF) - 1) & 0x3FF)` through a re-read `D_8009D278->f68`.
