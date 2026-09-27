# func_80020F18

- VRAM `0x80020F18`, file offset `0x11718`, size `0x13C` (79 words)
- Profile: `era_o2_g8_force_d8009d2fc_d8009d278_d8009d1ac_d8009d1ce_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2FC,D_8009D278,D_8009D1AC,D_8009D1CE (new profile)
- Source: `src/func_80020F18.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80020F18
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80020F18
LEAF_STRONG=EXACT func_80020F18 span=0x13C vram=0x80020F18
```

## Load-bearing levers

Two addressing levers: (1) in the 45-slot clear, `(&D_800BE830[i])->f6 = 0` makes cc1 hoist `&D_800BE830` into `$a1` and store `6(base)`, while `D_800BE830[i].f0`/`.f4` stay symbol+index `$at` stores (a pointer local `t[i].f6` or plain `D_800BE830[i].f6` both miss); (2) `D_8009CE38` is a **4-byte array**: the in-struct byte stores then block hoisting the `D_8009D278->f68` reload, which matches retail's reload-after-each-store (four scalar bytes let cc1 hoist the reload, costing 8 words). `-G8` for the gp scalars.
