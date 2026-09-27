# func_800136C0

- VRAM `0x800136C0`, file offset `0x3EC0`, size `0x2C8` (178 words)
- Profile: `era_o2_g8_force_d8009d2f0_d8009d254_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2F0,D_8009D254 (new profile, defined in lane1 private JSON)
- Source: `src/func_800136C0.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_800136C0
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_800136C0
LEAF_STRONG=EXACT func_800136C0 span=0x2C8 vram=0x800136C0
```

## Load-bearing levers

Crashed-session draft verified unchanged. Camera walk toward a target point; `-G8` keeps `D_8009D300`/`D_8009CE00` gp-relative while `D_8009D2F0` and `D_8009D254` must be absolute. See the source header for the per-block levers.
