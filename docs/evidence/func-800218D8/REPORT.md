# func_800218D8

- VRAM `0x800218D8`, file offset `0x120D8`, size `0x220` (136 words)
- Profile: `era_o2_g0`; maspsx env: none
- Source: `src/func_800218D8.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_800218D8
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_800218D8
LEAF_STRONG=EXACT func_800218D8 span=0x220 vram=0x800218D8
```

## Load-bearing levers

Field-by-field copy of the default record `D_800A76D8` into `D_8009D278->f68`, declared as **bitfields**: fC = 10/10/2 bits, f10 = 4/2/2/1/1/1/1/1/2/1 then **`unsigned short d10 : 1`** at bit 16 (makes cc1 read it with `lhu +2; andi 1; sll 16`, exactly as retail) and `unsigned int d11 : 1` at bit 17 (read with `lw` and mask). The per-field RMW keeps the destination word in a register across stores, and the large masks are hoisted into `$t0..$t4`, both as in retail.
