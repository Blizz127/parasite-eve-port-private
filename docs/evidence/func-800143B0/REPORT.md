# func_800143B0

- VRAM `0x800143B0`, file offset `0x4BB0`, size `0x14C` (83 words)
- Profile: `era_o2_g0`; maspsx env: none
- Source: `src/func_800143B0.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_800143B0
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_800143B0
LEAF_STRONG=EXACT func_800143B0 span=0x14C vram=0x800143B0
```

## Load-bearing levers

Same walk template; found tail is a Manhattan distance written with `#define ABS(x) ((x) < 0 ? -(x) : (x))` over the three `f28/f2C/f30` deltas (`=` then two `+=`). cc1 folds `-(a-b)` to `b-a`, reproducing retail's reversed `subu` in the delay slot; explicit if/else spellings miss by 83 words.
