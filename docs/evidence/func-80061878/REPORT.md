# func_80061878 — matching C leaf

- VRAM `0x80061878`, ROM offset `0X52078`, span `0x1C4`
- Source: `src/func_80061878.c`; YAML row `[0X52078, c, func_80061878]`
- Build profile: `era_o2_g8`

## Levers
Wipe-bar emitter: two DR_MODE prims (func_80077C84 with semi-transparency ((mode+1)&3)<<5 and ((2-mode)&3)<<5) bracketing two runs of func_8006153C rectangles read from a signed-byte list terminated by a negative byte; each DR_MODE is addPrim'd after its run (Tag bitfield). Starting point was the orphan draft (107 words off only because it was compiled without -G8: the D_8009D100 prim pool and D_8009D11C OT pointer are gp-relative); the one lever needed beyond -G8 is precomputing `m1 = mode + 1` before the first allocation and `m2 = 2 - mode` before the second, as retail keeps them in $s2/$s4.

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_80061878
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_80061878
  -> LEAF_STRONG=EXACT func_80061878 span=0x1C4 failures=[none]
```

