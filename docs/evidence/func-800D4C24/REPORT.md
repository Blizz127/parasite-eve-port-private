# func_800D4C24 — matching C leaf

- VRAM: `0x800D4C24`  size: `0x280`
- File offset: `0XC5424`
- Profile: `era_o2_g0`
- Source: `src/func_800D4C24.c`
- YAML: YAML [0xC5424, c, func_800D4C24]

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_800D4C24
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_800D4C24
LEAF_STRONG=EXACT func_800D4C24 span=0x280 vram=0x800D4C24 compiled=0x280 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- crashed-session draft verified as-is
- register pins h=$4 g=$2 in case 2 short-store block
