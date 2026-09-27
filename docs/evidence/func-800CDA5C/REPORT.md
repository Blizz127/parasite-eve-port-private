# func_800CDA5C — matching C leaf

- VRAM: `0x800CDA5C`  size: `0x1C8`
- File offset: `0XBE25C`
- Profile: `era_o2_g0`
- Source: `src/func_800CDA5C.c`
- YAML: YAML [0xBE25C, c][0xBE424, c]

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_800CDA5C
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_800CDA5C
LEAF_STRONG=EXACT func_800CDA5C span=0x1C8 vram=0x800CDA5C compiled=0x1D0 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- register int v asm("$2") for the rand() result held across stores (unpinned: extra move $4,$2)
- src = &D_800E27F0 local for first load, separate q = src copy before the loop (retail move $s4,$s0)
- i = 0; q = src; for (; ...) ordering
- index-form a2 + i*2 + 0x10.. (a p local reverses the addu operands)
- unsigned short i
