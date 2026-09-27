# func_8006153C — matching C leaf

- VRAM `0x8006153C`, ROM offset `0X51D3C`, span `0x33C`
- Source: `src/func_8006153C.c`; YAML row `[0X51D3C, c, func_8006153C]`
- Build profile: `era_o2_g8`

## Levers
Two-POLY_F3 bevel between two D_800A22B0 table points (index & 0x7F, bit 0x80 flips the offset sign), colour from D_8009D14C[sel], halved on the vertical edge. Same D_8009D100 pool/Tag addPrim conventions as func_80062090 (-G8). Levers: (1) pool pointer and base read into locals before the abs() branch (retail loads them first); (2) `ad = abs(d)` via the libc builtin (ABS_EXPR expansion tests $s2=d; every if/ternary spelling tests the copy: +1 word shift); (3) a separate loop pointer for the 2-prim init loop and `q = p` for the tail (retail's `move a3,s0`); (4) signed-offset updates through int temps (`t = b.x; b.x = (i1 & 0x80) ? t + ad : t - ad;`), a second temp for the vertical arm; (5) the vertical-arm colour stores the halved expression directly (a shared `c` lets cross-jump merge both arms' stores); (6) else-arm statement order a.x, b.x, a.y; (7) chained vertex stores written `q[0].x1 = q[1].x2 = b0.x` (right-to-left store order).

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8006153C
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8006153C
  -> LEAF_STRONG=EXACT func_8006153C span=0x33C failures=[none]
```

