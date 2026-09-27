# func_80013E84

- VRAM `0x80013E84`, file offset `0x4684`, size `0x3A4` (233 words)
- Profile: `era_o2_g8_force_d8009d2f0_d8009d254_d8009d20c_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2F0,D_8009D254,D_8009D20C (new profile)
- Source: `src/func_80013E84.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80013E84
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80013E84
LEAF_STRONG=EXACT func_80013E84 span=0x3A4 vram=0x80013E84
```

## Load-bearing levers

Crashed-session draft verified unchanged: func_80013C34 search head spliced onto func_800136C0 movement tail. The retail asm carries an interior `alabel func_80014000` that nothing references; the `c` span covers the whole 0x3A4.
