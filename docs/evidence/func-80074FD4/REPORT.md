# func_80074FD4 — matched (lane 4, session 2026-09-22)

- VRAM `0x80074FD4`, size `0x98`, source `src/func_80074FD4.c`.
- Profile: `era_o2_g0_cc1_281` (-O2 -G0 -mno-split-addresses, env ERA_CC1_VER=2.8.1).

## Verification (private lane config)

```text
python3 tools/build/disc1_preflight.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json --deep --only func_80074FD4
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json func_80074FD4
disc1_preflight: PASS (deep, 1661 c / 339 asm / 2 rodata)
LEAF_STRONG=EXACT func_80074FD4 span=0x98 vram=0x80074FD4 compiled=0xA0 failures=[none]
```

## Load-bearing levers

`unsigned char` byte parameters (the andi 0xFF masks are the incoming-arg truncations; int params + explicit masks reorder the prologue, 9 words). 2.8.1 band compiler fills the epilogue slot; 2.7.2 + MASPSX_FILL_EPILOGUE_DELAY_SLOT also matches, 2.8.1 chosen as the band default. D_80095744 is a pointer global, one base for +0x8 handler and v[3].
