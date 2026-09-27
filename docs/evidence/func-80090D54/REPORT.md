# func_80090D54 — matching C leaf

- VRAM: `0x80090D54`  size: `0xCC`
- File offset: `0X81554`
- Profile: `era_o2_g0`
- Source: `src/func_80090D54.c`
- YAML: YAML [0x81554, c][0x81620 ...]

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_80090D54
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_80090D54
LEAF_STRONG=EXACT func_80090D54 span=0xCC vram=0x80090D54 compiled=0xD0 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- second param a1 forwarded to func_800903A0 (retail never touches $a1, so locals start at $a2)
- for(; bit & 0xFFFFFF; bit <<= 1, i++) { if (!(m & bit)) break; } free-voice scan
