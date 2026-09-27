# func_80076170 — matched (lane 4, session 2026-09-22)

- VRAM `0x80076170`, size `0x98`, source `src/func_80076170.c`.
- Profile: `era_o2_g0_cc1_281` (-O2 -G0 -mno-split-addresses, env ERA_CC1_VER=2.8.1).

## Verification (private lane config)

```text
python3 tools/build/disc1_preflight.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json --deep --only func_80076170
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json func_80076170
disc1_preflight: PASS (deep, 1660 c / 339 asm / 2 rodata)
LEAF_STRONG=EXACT func_80076170 span=0x98 vram=0x80076170 compiled=0xA0 failures=[none]
```

## Load-bearing levers

Band compiler: ERA_CC1_VER=2.8.1 with -mno-split-addresses (profile era_o2_g0_cc1_281). Short params reassigned through nested ?: clamps; the value path reloads the extent as lhu (HImode) while the compare uses lh. 2.7.2 at -O1/-O2 allocates a 16-byte stack temp for the short ?: (27-36 words); 2.8.1 -O1 is 27 words off.
