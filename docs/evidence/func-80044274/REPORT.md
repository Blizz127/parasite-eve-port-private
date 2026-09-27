# func_80044274

- VRAM: `0x80044274`, size `0x1D0`
- Profile: `era_o2_g8_force_d8009cf90_d8009cf94_absolute`
- Source: `src/func_80044274.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_80044274
disc1_preflight: PASS (deep, 1677 c / 339 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_80044274
LEAF_STRONG=EXACT func_80044274 span=0x1D0 vram=0x80044274 compiled=0x1D0 failures=[none]
```

## Load-bearing spelling levers

NEW profile `-O2 -G8` + MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009CF90,D_8009CF94; the duplicated list-open block is a `static __inline__` helper expanded in both result arms; `(i < func_80052F70()) ? i : 0` gives retail's slt/negu/and; FLAG: the gp-relative store to D_8009CF90 is spelled `(&D_8009CF8C)[1] = ...` because retail's `la` of a small-data symbol is absolute but its store is gp-relative (no maspsx knob models la-absolute yet).

Item/equipment list refresh after func_800562A4. NEEDS REVIEW: symbol-alias store spelling (see note).
