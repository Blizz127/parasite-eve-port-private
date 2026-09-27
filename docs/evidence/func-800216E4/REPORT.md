# func_800216E4

- VRAM `0x800216E4`, file offset `0x11EE4`, size `0x16C` (91 words)
- Profile: `era_o2_g0`; maspsx env: none
- Source: `src/func_800216E4.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_800216E4
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_800216E4
LEAF_STRONG=EXACT func_800216E4 span=0x16C vram=0x800216E4
```

## Load-bearing levers

Recursive K&R quicksort over the 12-byte `D_8009E000` target table, keyed on `f4`; `signed char lo, hi` parameters and `signed char i, last` locals give the `sll/sra 24` re-extensions; the partition loop starts at **`i = lo`** (not `lo + 1`), and the key compares read the global table by index (the `a` argument is only forwarded to the swap helper `func_80021850`).
