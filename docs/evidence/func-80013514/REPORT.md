# func_80013514

- VRAM `0x80013514`, file offset `0x3D14`, size `0x1AC` (107 words)
- Profile: `era_o2_g8_force_d8009d2f0_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2F0 (via profile)
- Source: `src/func_80013514.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80013514
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80013514
LEAF_STRONG=EXACT func_80013514 span=0x1AC vram=0x80013514
```

## Load-bearing levers

Point-target twin of func_80013C34 (the turn/step tail is reused verbatim). Levers: natural load order (`px`, `pz`, `step`) with the `fl | 0x20` flag store **before** the three latch stores (an exhaustive 144-permutation sweep found this as the unique match); the deltas **reuse** the `px`/`pz` locals (`px = (D->f28 - px) >> 16`), which puts them in `$a2`/`$a0` and gives retail's `move $a1,$a2` for the call; the `(px | pz) == 0` early return.
