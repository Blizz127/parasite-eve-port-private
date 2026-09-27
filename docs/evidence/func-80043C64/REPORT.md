# func_80043C64

- VRAM: `0x80043C64`, size `0x140`
- Profile: `era_o2_g8`
- Source: `src/func_80043C64.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_80043C64
disc1_preflight: PASS (deep, 1665 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_80043C64
LEAF_STRONG=EXACT func_80043C64 span=0x140 vram=0x80043C64 compiled=0x140 failures=[none]
```

## Load-bearing spelling levers

`-O2 -G8` (D_8009CF40 at 0x1D0($gp)); `p = func_80051E48() + 1` int* walker advanced mid-body; out param a single `int` (frame 0x30); magnitude spelled `d >= 0 ? d : -d` (the `d < 0 ? -d : d` form and an `if`-negate both mis-schedule the `bgez`/`negu`).

Status-screen stat row printer for stats 1..6 with signed delta arrows (0x6F/0x70 glyphs).
