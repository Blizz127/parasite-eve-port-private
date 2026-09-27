# func_80016F10

- VRAM `0x80016F10`, file offset `0x7710`, size `0xD0` (52 words)
- Profile: `era_o2_g8_force_d8009d1a0_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D1A0 (via profile)
- Source: `src/func_80016F10.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80016F10
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80016F10
LEAF_STRONG=EXACT func_80016F10 span=0xD0 vram=0x80016F10
```

## Load-bearing levers

Sibling of func_80015BAC with the flag test inverted (`if (t->flags & 0x20)` call arm first) and a shared `D_8009D300->f10 = 1` tail; always returns 0. `D_800B0CD8` unsized array, `D_8009D1A0` forced absolute, `-G8`.
