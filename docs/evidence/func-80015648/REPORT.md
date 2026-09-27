# func_80015648

- VRAM `0x80015648`, file offset `0x5E48`, size `0x148` (82 words)
- Profile: `era_o2_g0`; maspsx env: none
- Source: `src/func_80015648.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80015648
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80015648
LEAF_STRONG=EXACT func_80015648 span=0x148 vram=0x80015648
```

## Load-bearing levers

Actor-walk `goto` template (block-local `q` null test) with a `*arg0->f8 = -1` fail block; found tail is the func_80019C4C heading formula. Named `y` (f28 delta) **then** `x` (f30 delta) locals before `func_80079FB4(x, y)` fix the load order (inline argument expressions load f30 first: 6 words).
