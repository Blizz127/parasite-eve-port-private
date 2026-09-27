# func_8004542C

- VRAM: `0x8004542C`, size `0xF0`
- Profile: `era_o2_g8`
- Source: `src/func_8004542C.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_8004542C
disc1_preflight: PASS (deep, 1656 c / 342 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_8004542C
LEAF_STRONG=EXACT func_8004542C span=0xF0 vram=0x8004542C compiled=0xF0 failures=[none]
```

## Load-bearing spelling levers

`-O2 -G8` so D_8009CF1C/D_8009CF18 stay gp-relative (0x1AC/0x1A8($gp)); read D_8009CF1C into a local `flag` BEFORE the three handler-pointer stores (retail loads it right after the second call); else-if chain for the 80064C20 / 80064B74(q,0x12) choice.

Spawner: func_80062D2C(5,a0,0,0) + two func_8006322C children (5 and 0x1B).
