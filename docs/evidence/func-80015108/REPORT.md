# func_80015108

- VRAM `0x80015108`, file offset `0x5908`, size `0x138` (78 words)
- Profile: `era_o2_g0`; maspsx env: none
- Source: `src/func_80015108.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80015108
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80015108
LEAF_STRONG=EXACT func_80015108 span=0x138 vram=0x80015108
```

## Load-bearing levers

Same `goto fail`/`goto found` actor-walk template as func_80019170 (block-local `Node *q` null test). The found tail uses one block-local `s = D_8009D2F0` for the `f18C`/`f28..f30` (`f254..f258 << 16`) stores and the `f98 |= 0x2000`.
