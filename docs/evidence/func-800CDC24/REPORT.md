# func_800CDC24 — matching C leaf

- VRAM: `0x800CDC24`  size: `0xE0`
- File offset: `0XBE424`
- Profile: `era_o2_g0`
- Source: `src/func_800CDC24.c`
- YAML: YAML [0xBE424, c]

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_800CDC24
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_800CDC24
LEAF_STRONG=EXACT func_800CDC24 span=0xE0 vram=0x800CDC24 compiled=0xE0 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- store order 4,6,8 then a2[1]=0x7F (byte store aliases the global loads, so source order is visible)
- v = rand(); a2->E = 0; a2->C = v % 101 - 50 puts the zero store in the mult shadow
