# func_80045D0C

- VRAM: `0x80045D0C`, size `0x1D8`
- Profile: `era_o2_g8`
- Source: `src/func_80045D0C.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_80045D0C
disc1_preflight: PASS (deep, 1678 c / 339 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_80045D0C
LEAF_STRONG=EXACT func_80045D0C span=0x1D8 vram=0x80045D0C compiled=0x1E0 failures=[none]
```

## Load-bearing spelling levers

`-O2 -G8` (D_8009CF1C gp-relative); variable REUSE is load-bearing: the cancel (0x40) branch reassigns the parameter `a0 = func_80062A34(1, 6)` (retail keeps it in $s2), and the 0x4000 branch reassigns `p` for both follow-up lookups; the spawn branch reuses `q` for the func_8006322C child.

Menu input handler: confirm / cancel / 0x4000 button actions.
