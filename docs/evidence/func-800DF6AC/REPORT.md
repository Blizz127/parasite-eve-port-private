# func_800DF6AC — matching C leaf

- VRAM: `0x800DF6AC`  size: `0x1D0`
- File offset: `0XCFEAC`
- Profile: `era_o2_g0`
- Source: `src/func_800DF6AC.c`
- YAML: YAML carve [0xCDB80, asm][0xCFEAC, c, func_800DF6AC]

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_800DF6AC
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_800DF6AC
LEAF_STRONG=EXACT func_800DF6AC span=0x1D0 vram=0x800DF6AC compiled=0x1D0 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- crashed-session draft verified as-is
- register pin h=$2 in case 0
