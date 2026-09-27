# func_800144FC

- VRAM `0x800144FC`, file offset `0x4CFC`, size `0x198` (102 words)
- Profile: `era_o2_g8_dispatch_800100a0_force_d800b0cd8_d8009d1a0_absolute`; maspsx env: MASPSX_THREE_WORD_SYMBOL_STORE=1, MASPSX_DISPATCH_FOLD=jtbl_800100A0, MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800B0CD8,D_8009D1A0 (new profile; three-word knob is load-bearing, 1 word without)
- Source: `src/func_800144FC.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_800144FC
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_800144FC
LEAF_STRONG=EXACT func_800144FC span=0x198 vram=0x800144FC
```

## Load-bearing levers

Field-VM state machine over the `D_800B0CD8+0xF4` state byte, a `switch` folded onto the retail table `jtbl_800100A0` (cases 0, 0x37..0x3B; 54 default slots). **The re-dispatch must be a `goto top` label loop, not `for (;;)`**: a real loop lets loop.c hoist `0x400000` and `1` into `$s1/$s2` (97 words). Case 0x3B `break`s to the shared `return 1` (a separate `return 1` duplicates the stub). The system block is `(Sys *)&D_800B0CD8` held in `$s0`, while the two `0x400000` tests read the scalar `D_800B0CD8` absolutely.
