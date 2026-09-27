# func_800CC55C — matching C leaf

- VRAM: `0x800CC55C`  size: `0xE8`
- File offset: `0XBCD5C`
- Profile: `era_o2_g0`
- Source: `src/func_800CC55C.c`
- YAML: first try; twin of func_800CC7AC

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_800CC55C
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_800CC55C
LEAF_STRONG=EXACT func_800CC55C span=0xE8 vram=0x800CC55C compiled=0xF0 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- single short-array symbol D_800E2260 with element/word-cast offsets (first store shares the la base, later ones go via lui $at)
- index-form a2 + i*8 + 0x26 reads
- unsigned i vs signed char count -> sltu
