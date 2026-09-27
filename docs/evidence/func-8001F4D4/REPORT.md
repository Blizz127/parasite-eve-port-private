# func_8001F4D4

- VRAM `0x8001F4D4`, file offset `0xFCD4`, size `0x340` (208 words)
- Profile: `era_o2_g0_expand_div`; maspsx env: MASPSX_EXPAND_DIV=1 (via profile)
- Source: `src/func_8001F4D4.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_8001F4D4
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_8001F4D4
LEAF_STRONG=EXACT func_8001F4D4 span=0x340 vram=0x8001F4D4
```

## Load-bearing levers

Crashed-session draft verified unchanged; `MASPSX_EXPAND_DIV=1` is load-bearing for the inline ASPSX divide guard (`(x << 2) / short`). Without it: 25 word mismatches.
