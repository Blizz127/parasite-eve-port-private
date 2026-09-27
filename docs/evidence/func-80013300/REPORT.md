# func_80013300

- VRAM `0x80013300`, file offset `0x3B00`, size `0xE8` (58 words)
- Profile: `era_o2_g8_force_d8009d2f0_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2F0 (via profile)
- Source: `src/func_80013300.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80013300
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80013300
LEAF_STRONG=EXACT func_80013300 span=0xE8 vram=0x80013300
```

## Load-bearing levers

From the crashed-session draft, simplified: the `asm volatile` memory barrier and `volatile` casts were **not** load-bearing and were removed. Levers: a `flags` local read once (reused for both the OR-set and the `& 0xFFDF` clear); mirrored `left >= right` compares in the two `field_1C` sign arms with a shared `goto yield`; `-G8` for `D_8009D300`/`D_8009CE00` with `D_8009D2F0` forced absolute.
