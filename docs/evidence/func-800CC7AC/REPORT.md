# func_800CC7AC — matching C leaf

- VRAM: `0x800CC7AC`  size: `0xCC`
- File offset: `0XBCFAC`
- Profile: `era_o2_g0`
- Source: `src/func_800CC7AC.c`
- YAML: YAML [0xBCFAC, c, func_800CC7AC]

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_800CC7AC
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_800CC7AC
LEAF_STRONG=EXACT func_800CC7AC span=0xCC vram=0x800CC7AC compiled=0xD0 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- (short)(c << 8) >> 7 from an unsigned char local gives lbu+sll24+sra23
- index-form reads a2 + i*2 + 8/0x28/0x48 keep retail displacements (a walking pointer gets giv-biased by +72)
- unsigned i vs short count -> sltu + beqz guard
- single symbol D_800E2818 with [0x14] element so &D_800E2840 and base share one la
