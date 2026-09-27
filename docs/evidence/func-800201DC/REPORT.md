# func_800201DC

- VRAM `0x800201DC`, file offset `0x109DC`, size `0xAC` (43 words)
- Profile: `era_o2_g0`; maspsx env: none
- Source: `src/func_800201DC.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_800201DC
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_800201DC
LEAF_STRONG=EXACT func_800201DC span=0xAC vram=0x800201DC
```

## Load-bearing levers

Record struct typing of `D_8009D278` (0x0C/0x0E/0x38/0x3A/0x3B/0x60..0x67); local `p` for the head block, then plain `D_8009D278->` for the tail stores so cc1 re-loads the global after each intervening store exactly as retail.
