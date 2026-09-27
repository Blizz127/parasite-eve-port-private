# func_800327D8

- VRAM: `0x800327D8`, size `0x104`
- Profile: `era_o2_g0`
- Source: `src/func_800327D8.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_800327D8
disc1_preflight: PASS (deep, 1659 c / 343 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_800327D8
LEAF_STRONG=EXACT func_800327D8 span=0x104 vram=0x800327D8 compiled=0x110 failures=[none]
```

## Load-bearing spelling levers

Record typing: D_8009E880[] = {DR 8 bytes; SPRT 20 bytes} (28-byte stride) — storing through `s = &D_8009E880[k].spr` makes cc1 CSE `la D_8009E880+8` and derive the record base with `addiu -8` exactly as retail; statement order `y`, `s`, `x` (the only one of 6 permutations that allocates like retail); Tile x/y are `unsigned short` (lhu); k = D_8009CDDC in a local for the first half, re-read global for the second call.

Draws a SPRT offset (+8, +a1) from the frame Tile origin, addprim at OT+0x10, then func_800328DC(&D_8009E8B8[k], x+0x40, y-1, a0, 1).
