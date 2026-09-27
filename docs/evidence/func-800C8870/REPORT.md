# func_800C8870 — matching C leaf

- VRAM: `0x800C8870`  size: `0x100`
- File offset: `0XB9070`
- Profile: `era_o2_g0`
- Source: `src/func_800C8870.c`
- YAML: YAML carve [0xB8BA0 asm][0xB9070 c]

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_800C8870
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_800C8870
LEAF_STRONG=EXACT func_800C8870 span=0x100 vram=0x800C8870 compiled=0x100 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- identity MATRIX via chained assignments (m00=m11=m22=0x1000; t0=t1=t2=0; off-diagonals chain) reproduces the reversed store order
- a2 field reads hoisted into locals s/u before the matrix init (loads may alias the stack stores, so source order is visible)
- D_800E2318[5] store; base shared with func_800C42A4 arg
