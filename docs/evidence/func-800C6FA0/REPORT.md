# func_800C6FA0 — matching C leaf

- VRAM: `0x800C6FA0`  size: `0xF8`
- File offset: `0XB77A0`
- Profile: `era_o2_g0`
- Source: `src/func_800C6FA0.c`
- YAML: YAML [0xB77A0, c]

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_800C6FA0
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_800C6FA0
LEAF_STRONG=EXACT func_800C6FA0 span=0xF8 vram=0x800C6FA0 compiled=0x100 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- scratchpad word through a base pointer s = (unsigned int *)0x1F800000; s[12] (a literal 0x1F800030 address folds to li + 0 offset)
- c += 4 in the for header after i++
- unsigned short scale param -> andi 0xFFFF
