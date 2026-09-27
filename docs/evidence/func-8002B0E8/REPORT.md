# func_8002B0E8

- VRAM `0x8002B0E8`, file offset `0x1B8E8`, size `0x1B4` (109 words)
- Profile: `era_o2_g8_force_d8009d254_d8009d1a0_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D254,D_8009D1A0 (new profile)
- Source: `src/func_8002B0E8.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_8002B0E8
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_8002B0E8
LEAF_STRONG=EXACT func_8002B0E8 span=0x1B4 vram=0x8002B0E8
```

## Load-bearing levers

Four-state `switch` on the gp byte `D_8009CE74` (small switch, compare ladder, no table). `-G8` keeps `D_8009CE74`, `D_8009D304`, `D_8009D21C`, `D_8009D2A4` and `D_8009D28C` gp-relative, with `D_8009D254`/`D_8009D1A0` forced absolute. Branch polarity is load-bearing in case 2: the `!(D_8009D1A0 & 0x1800)` arm (k = 0x15) must come first (7 words otherwise). The shared constant 1 lands in `$s0` across the case-3 call.
