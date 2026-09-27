# func_800362B8

- VRAM: `0x800362B8`, size `0x13C`
- Profile: `era_o2_g0`
- Source: `src/func_800362B8.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_800362B8
disc1_preflight: PASS (deep, 1663 c / 342 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_800362B8
LEAF_STRONG=EXACT func_800362B8 span=0x13C vram=0x800362B8 compiled=0x140 failures=[none]
```

## Load-bearing spelling levers

Slot[16] {count, ptr} table; size ladder spelled `n = 8; if (size < 0x47E1) n = 1; else if ... ` (default in the first delay slot); `unsigned char` n and i, `unsigned int` j (strength-reduced byte-offset biv + `sltu`); D_800B0E4C typed as an ARRAY `unsigned char *D_800B0E4C[]` read as `D_800B0E4C[0]` — this is what makes cc1 hoist `la $t1,D_800B0E4C` out of the loop and load `lw 0($t1)` (a scalar pointer global loads absolute).

VRAM slot allocator: 1/2/4/8 contiguous 0x47E0-byte slots; returns the slot pointer or 0.
