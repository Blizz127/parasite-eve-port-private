# func_800CC480 — matching C leaf

- VRAM: `0x800CC480`  size: `0xDC`
- File offset: `0XBCC80`
- Profile: `era_o2_g0`
- Source: `src/func_800CC480.c`
- YAML: YAML [0xBCC80 c][0xBCD5C c]

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_800CC480
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_800CC480
LEAF_STRONG=EXACT func_800CC480 span=0xDC vram=0x800CC480 compiled=0xE0 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- register unsigned char *p asm("$17") = a2 as the first statement moves the a2 copy into the first jal delay slot (plain param: li $a0,3 lands there)
- short v with if (v < 0) v = 0 clamp
- single D_800F3430 symbol
