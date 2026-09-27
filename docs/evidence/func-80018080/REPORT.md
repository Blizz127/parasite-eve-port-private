# func_80018080

- VRAM `0x80018080`, file offset `0x8880`, size `0xE4` (57 words)
- Profile: `era_o2_g0`; maspsx env: none
- Source: `src/func_80018080.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80018080
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80018080
LEAF_STRONG=EXACT func_80018080 span=0xE4 vram=0x80018080
```

## Load-bearing levers

Sibling of the already-matched func_800181CC actor-list matcher: same `goto loop` walk over `D_8009D20C` (`f0C` type key, `f0D` owner key, skip `f98 & 0x10`) — the goto form reproduces retail's duplicated `bnez` exit test; both arms store the callee result to `*arg0->fC`.
