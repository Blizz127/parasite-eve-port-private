# func_80085A64

- VRAM `0x80085A64`, size `0xCC` (51 words), file offset `0x76264`
- Profile: `era_o2_g0_cc1_281`, flags `-O2 -G0 -mno-split-addresses`, env `ERA_CC1_VER=2.8.1`
- Source: `src/func_80085A64.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_80085A64
  -> disc1_preflight: PASS (deep, 1666 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_80085A64
  -> LEAF_STRONG=EXACT
(ERA_CC1_VER=2.8.1 python3 tools/analysis/era_link_check.py src/func_80085A64.c 0x80085A64 0xCC -O2 -G0 -mno-split-addresses)
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

switch(a0) {0,1}; `D_8009B394 != a0` (not `!= 1`) reproduces `beq $v0,$s0`, and `D_8009B390 = a0` stores $s0.
