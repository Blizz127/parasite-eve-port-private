# func_80014FD8

- VRAM `0x80014FD8`, file offset `0x57D8`, size `0x130` (76 words)
- Profile: `era_o2_g8_force_d8009d2f0_d8009d254_d8009d2e8_d800b0d88_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2F0,D_8009D254,D_8009D2E8,D_800B0D88 (new profile)
- Source: `src/func_80014FD8.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80014FD8
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80014FD8
LEAF_STRONG=EXACT func_80014FD8 span=0x130 vram=0x80014FD8
```

## Load-bearing levers

`-G8` keeps `D_8009D300`/`D_8009CE00` gp-relative, with four symbols forced absolute. **Struct typing of the task record is load-bearing**: the `t->flags` halfword store is a MEM_IN_STRUCT store, so cc1 does not reload the scalar `D_8009D2F0` before the `addiu a0,a0,0x1B4` argument. `D_800B0CEC` is passed as an unsized-array address.
