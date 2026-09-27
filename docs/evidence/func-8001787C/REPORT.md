# func_8001787C

- VRAM `0x8001787C`, file offset `0x807C`, size `0xAC` (43 words)
- Profile: `era_o2_g8_force_d8009d2f0_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2F0 (via profile)
- Source: `src/func_8001787C.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_8001787C
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_8001787C
LEAF_STRONG=EXACT func_8001787C span=0xAC vram=0x8001787C
```

## Load-bearing levers

`-G8` for the `0x590($gp)` self pointer `D_8009D300`, with `D_8009D2F0` forced absolute; `unsigned char i` gives the `andi 0xFF`/`sltiu 3` loop counter; the two early exits return 0 (self hit) / 1.
