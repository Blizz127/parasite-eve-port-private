# func_80090E20 — matching C leaf

- VRAM: `0x80090E20`  size: `0x134`
- File offset: `0X81620`
- Profile: `era_o2_g0_expand_div`
- Source: `src/func_80090E20.c`
- YAML: YAML [0x81620, c, func_80090E20]

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_80090E20
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_80090E20
LEAF_STRONG=EXACT func_80090E20 span=0x134 vram=0x80090E20 compiled=0x140 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- (short)(c << 8) for lbu+sll24+sra16 (signed char gives lb)
- assignment-in-condition for the +0x60 store then 0x100 fallback
- a1 forwarded
- MASPSX_EXPAND_DIV=1
