# func_800CE78C — matching C leaf

- VRAM: `0x800CE78C`  size: `0xE4`
- File offset: `0XBEF8C`
- Profile: `era_o2_g0`
- Source: `src/func_800CE78C.c`
- YAML: YAML [0xBEF8C, c]

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_800CE78C
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_800CE78C
LEAF_STRONG=EXACT func_800CE78C span=0xE4 vram=0x800CE78C compiled=0xF0 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- pool walk: p advanced in the for header after i++ (in-body advance schedules addu before the i++)
- declaration order short *p before int n swaps the $s3/$s4 allocation to retail
