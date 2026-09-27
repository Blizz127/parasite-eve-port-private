# func_800471E4

- VRAM: `0x800471E4`, size `0x148`
- Profile: `era_o2_g8`
- Source: `src/func_800471E4.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_800471E4
disc1_preflight: PASS (deep, 1667 c / 340 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_800471E4
LEAF_STRONG=EXACT func_800471E4 span=0x148 vram=0x800471E4 compiled=0x150 failures=[none]
```

## Load-bearing spelling levers

`-O2 -G8` (D_8009CFB8 = 0x248($gp)); switch with `case 0: if (sel == 2) { spawn; break; }` falling through into `case 2: func_800473E4(p)`; spawned actor in a fresh local `r` (reusing `p` swaps the `li a0,0x3B`/`move s0` order); `register int sel asm("$5")` pin (retail keeps the func_80063428 result in $a1; unpinned costs 3 words).

Menu input handler: on confirm (bit 16) dispatch by mode D_8009CFB8, on cancel (0x40) close.
