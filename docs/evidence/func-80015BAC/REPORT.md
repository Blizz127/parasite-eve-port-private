# func_80015BAC

- VRAM `0x80015BAC`, file offset `0x63AC`, size `0xD0` (52 words)
- Profile: `era_o2_g8_force_d8009d1a0_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D1A0 (via profile)
- Source: `src/func_80015BAC.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80015BAC
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80015BAC
LEAF_STRONG=EXACT func_80015BAC span=0xD0 vram=0x80015BAC
```

## Load-bearing levers

`-G8` for the gp-relative `D_8009D300` (0x590) / `D_8009CE00` (0x90) with `D_8009D1A0` forced absolute; `D_800B0CD8` declared as an **unsized array** (`extern unsigned int D_800B0CD8[]`) and accessed as `D_800B0CD8[0]` directly: keeps the address in `$s0` (not small-data) while the store stays a known-symbol store, so cc1 may hoist the `D_8009D300` reload above it (a `unsigned int *f` local blocks that hoist; a scalar extern under -G8+force-absolute folds the base).
