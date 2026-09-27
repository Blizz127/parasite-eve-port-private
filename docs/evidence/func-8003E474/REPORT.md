# func_8003E474

- VRAM: `0x8003E474`, size `0x17C`
- Profile: `era_o2_g0`
- Source: `src/func_8003E474.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_8003E474
disc1_preflight: PASS (deep, 1669 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_8003E474
LEAF_STRONG=EXACT func_8003E474 span=0x17C vram=0x8003E474 compiled=0x180 failures=[none]
```

## Load-bearing spelling levers

POLY_GT4/POLY_GT3 struct typing walked two buffers per polygon; `signed char dc` PARAMETER (an `int` param with `(signed char)` casts keeps a different register for the shifted copy); uniform v0,u0,v1,u1,v2,u2,(v3),clut statement order in both loops (makes the biv base +0xE clut); inner loop `for (j = 0; j < 2; j++, p++)` (j increment before the pointer increment).

Scrolls a mesh's texture coords by (du,dv) and shifts CLUT by dc across both double-buffered prim copies.
