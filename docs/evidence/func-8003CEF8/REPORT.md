# func_8003CEF8

- VRAM: `0x8003CEF8`, size `0x158`
- Profile: `era_o2_g0`
- Source: `src/func_8003CEF8.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_8003CEF8
disc1_preflight: PASS (deep, 1668 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_8003CEF8
LEAF_STRONG=EXACT func_8003CEF8 span=0x158 vram=0x8003CEF8 compiled=0x160 failures=[none]
```

## Load-bearing spelling levers

Mesh {hdr,+0x54 prims,+0xBA} typing; prims walked as a byte pointer (+0x68 per double-buffered POLY_GT4, +0x50 per POLY_GT3) with the frame-k copy picked by `q = p + k * 52` / `q = p + k * 40` in a named local inside the loop (inline `p + k*52 + 0x1A` swaps the addu operands); delta read into an `int` (lh, not lhu); hdr counts re-read every iteration.

Retargets the semi-transparency mode bits of every mesh polygon's tpage by the D_800921D8[cur][mode] delta.
