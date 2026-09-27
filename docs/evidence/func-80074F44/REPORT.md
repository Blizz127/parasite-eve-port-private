# func_80074F44 — matched (lane 4, session 2026-09-22)

- VRAM `0x80074F44`, size `0x90`, source `src/func_80074F44.c`.
- Profile: `era_o2_g0_cc1_281` (-O2 -G0 -mno-split-addresses, env ERA_CC1_VER=2.8.1).

## Verification (private lane config)

```text
python3 tools/build/disc1_preflight.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json --deep --only func_80074F44
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json func_80074F44
disc1_preflight: PASS (deep, 1662 c / 339 asm / 2 rodata)
LEAF_STRONG=EXACT func_80074F44 span=0x90 vram=0x80074F44 compiled=0x90 failures=[none]
```

## Load-bearing levers

`unsigned char` byte parameters close the 9-word prologue-interleave residual recorded in PARK.md (int params + explicit `& 0xFF` masks schedule the `la $a0` late). 2.8.1 band compiler (fills the epilogue slot without patch 3). Twin of func_80074FD4 (which ORs 0x80000000).
