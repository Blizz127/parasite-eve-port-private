# func_8007512C — matched (lane 4, session 2026-09-22)

- VRAM `0x8007512C`, size `0xB8`, source `src/func_8007512C.c`.
- Profile: `era_o2_g0_cc1_281` (-O2 -G0 -mno-split-addresses, env ERA_CC1_VER=2.8.1).

## Verification (private lane config)

```text
python3 tools/build/disc1_preflight.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json --deep --only func_8007512C
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json func_8007512C
disc1_preflight: PASS (deep, 1663 c / 338 asm / 2 rodata)
LEAF_STRONG=EXACT func_8007512C span=0xB8 vram=0x8007512C compiled=0xC0 failures=[none]
```

## Load-bearing levers

Packet as a struct global `PKT D_800957E4` (tag, cmd, xy, col, wh); store order xy -> col -> wh makes cc1 anchor the base on D_800957E4+8 (= retail D_800957EC) and pass base-8; col-first (the retail store order) anchors on +0xC (14 words). `int` params with `& 0xFFFF` (unsigned short params reorder the prologue, 21 words).
