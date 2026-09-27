# func_80042D40

- VRAM: `0x80042D40`, size `0x190`
- Profile: `era_o2_g8`
- Source: `src/func_80042D40.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_80042D40
disc1_preflight: PASS (deep, 1670 c / 340 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_80042D40
LEAF_STRONG=EXACT func_80042D40 span=0x190 vram=0x80042D40 compiled=0x190 failures=[none]
```

## Load-bearing spelling levers

`-O2 -G8` for the fade-state words; out-of-window D_800B0E50/54 and D_800BCExx/DExx declared as incomplete ARRAYS (`[0]`) so they stay absolute; pixel math with `v &= 0x8000` placed right after the three channel masks and a final `v = v | r | (g<<5) | (b<<10); *dst = v;`; `register unsigned short *dst asm("$8")` pin (unpinned costs 9 words of register numbering); tail broadcast through a separate local `x = (unsigned)w >> 13` stored in retail order.

15-bit CLUT fade: steps a fade table position and rescales every palette entry toward black, preserving the STP bit.
