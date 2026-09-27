# func_80021DE0

- VRAM `0x80021DE0`, file offset `0x125E0`, size `0x158` (86 words)
- Profile: `era_o2_g8_force_d8009d254_d8009d2e8_d8009d278_d8009d28c_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D254,D_8009D2E8,D_8009D278,D_8009D28C (new profile)
- Source: `src/func_80021DE0.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80021DE0
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80021DE0
LEAF_STRONG=EXACT func_80021DE0 span=0x158 vram=0x80021DE0
```

## Load-bearing levers

`-G8` keeps the byte cursor `D_8009D1D4` (0x464) and count `D_8009CE3C` (0xCC) gp-relative, with four symbols forced absolute; slot pointer `&D_800BE830[idx]` (unsized `Slot` array) taken before the three zero stores; `f4` range ladder as an if/else-if chain; `func_800255E4` returns **`signed char`** (plain `char` gives `andi 0xFF` instead of `sll/sra 24`).
