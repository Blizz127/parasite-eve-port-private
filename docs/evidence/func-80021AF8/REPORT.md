# func_80021AF8

- VRAM `0x80021AF8`, file offset `0x122F8`, size `0x254` (149 words)
- Profile: `era_o2_g0`; maspsx env: none
- Source: `src/func_80021AF8.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80021AF8
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80021AF8
LEAF_STRONG=EXACT func_80021AF8 span=0x254 vram=0x80021AF8
```

## Load-bearing levers

Second bitfield-copy twin of func_800218D8: `D_800A76D8` viewed as an 8-byte config (word 0 = 10/10/8/4 bits; word 1 = 1,1,1,1,1,4,1x7, then **`unsigned short : 1`** at bit 16 for retail's `lhu D_800A76DE; andi 1; sll 16`, then an int `: 1` at bit 17), copied field by field into `D_8009D278->f6C`. cc1 keeps word 0 through a `$t0` base and word 1 through the absolute `D_800A76DC` symbol, exactly as retail.
