# func_80089250 — matching C leaf

- VRAM: `0x80089250`  size: `0xD8`
- File offset: `0X79A50`
- Profile: `era_o2_g0`
- Source: `src/func_80089250.c`
- YAML: YAML [0x79A50 c][0x79B28 asm]

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_80089250
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_80089250
LEAF_STRONG=EXACT func_80089250 span=0xD8 vram=0x80089250 compiled=0xE0 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- loop-invariant bit source as a local b = 1 initialised in the for header after i (gives hoisted $s4=1 in retail order)
- index-form D_800B002C[i*4] (a walking pointer hoists 0x7FFF into a callee-saved reg)
- single OR expression ...|a0 for the mask
