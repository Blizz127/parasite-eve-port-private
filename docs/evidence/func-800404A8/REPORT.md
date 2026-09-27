# func_800404A8

- VRAM: `0x800404A8`, size `0xFC`
- Profile: `era_o2_g0`
- Source: `src/func_800404A8.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_800404A8
disc1_preflight: PASS (deep, 1658 c / 342 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_800404A8
LEAF_STRONG=EXACT func_800404A8 span=0xFC vram=0x800404A8 compiled=0x100 failures=[none]
```

## Load-bearing spelling levers

CardState[2] (0x418 stride) typing on the D_800A0EDC symbol; `(unsigned)(state-2) < 2` range test; after the loop `register int *p asm("$5") = D_800A1848` + the zero-code `asm volatile("" : "=r"(p) : "0"(p))` barrier keeps retail's re-materialised base register (without it cc1 folds p back to absolute lui/lw); clamp spelled `v = 4; if (D_800A1844 < 5) v = D_800A1844;`.

Card-poll timer: decrement D_800A1844, re-arm to 12 for slots in state 2/3, clear pending flags once both slots settle.
