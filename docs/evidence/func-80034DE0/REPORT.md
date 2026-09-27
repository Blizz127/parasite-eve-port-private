# func_80034DE0

- VRAM: `0x80034DE0`, size `0x130`
- Profile: `era_o2_g8`
- Source: `src/func_80034DE0.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_80034DE0
disc1_preflight: PASS (deep, 1662 c / 343 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_80034DE0
LEAF_STRONG=EXACT func_80034DE0 span=0x130 vram=0x80034DE0 compiled=0x130 failures=[none]
```

## Load-bearing spelling levers

`-O2 -G8` (D_8009D1CE/D_8009CE80/D_8009CE88/D_8009D1F8 are gp-relative); D_800BCEA8 typed as the unsized BceRecord[] so it stays absolute; switch on D_8009D1CE; scalar `short v = -1` passed by address (frame 0x20 — an array gives 0x28); statement order b0=2, CE88=0x4B, w4=D1F8, flags|=0x2000000, THEN D_8009D1CE++ (found by exhaustive 120-permutation sweep; only this order reproduces retail's early lbu of D_8009D1CE).

Two-state overlay/menu tick then the per-frame func_8005E894/func_80061C34 draw.
