# func_800425DC

- VRAM: `0x800425DC`, size `0x194`
- Profile: `era_o2_g0`
- Source: `src/func_800425DC.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_800425DC
disc1_preflight: PASS (deep, 1672 c / 338 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_800425DC
LEAF_STRONG=EXACT func_800425DC span=0x194 vram=0x800425DC compiled=0x1A0 failures=[none]
```

## Load-bearing spelling levers

switch on D_800A1864 with case 0 / case -1 / default (retail's beq -1 then bnez tree); card-slot loop `for (i = 1, c = &D_800A0ED4[1]; i >= 0; i--, c--)` pointer walk (i initialised BEFORE c gives retail's per-path `li s0,1` delay slots); CardState[2] typing on D_800A0EDC for the pending-slot state test.

Memory-card per-frame tick: poll both slots, resolve the pending save/load result, run each slot's handler.
