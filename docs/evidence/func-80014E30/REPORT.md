# func_80014E30

- VRAM `0x80014E30`, file offset `0x5630`, size `0x1A8` (106 words)
- Profile: `era_o2_g0`; maspsx env: none
- Source: `src/func_80014E30.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80014E30
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80014E30
LEAF_STRONG=EXACT func_80014E30 span=0x1A8 vram=0x80014E30
```

## Load-bearing levers

Movie-stage loader: two CD read stages, each a `while (func_8006E6A8(...) == -1) {}` issue loop plus a `while ((r = func_8006E7E8()) != 0) { if (r == -1) goto retryN; }` poll, which reproduces retail's `$s0/$s1` re-seeding at each retry label. `D_8009315E` is an unsigned-short offset table (entries 0/1/2), `D_8001160C` a two-entry buffer array; `(Sys *)D_800B0CD8` (unsized array) sits in `$s2`. The final request is a two-word stack struct (`f4 = 0` stored first). Calls into the `0x80121xxx` overlay (movie player) link as plain symbols.
