# func_800762BC — matched (lane 4, session 2026-09-22)

- VRAM `0x800762BC`, size `0x80`, source `src/func_800762BC.c`.
- Profile: `era_o2_g0_fill_epilogue_delay_slot` (-O2 -G0, env MASPSX_FILL_EPILOGUE_DELAY_SLOT=1).

## Verification (private lane config)

```text
python3 tools/build/disc1_preflight.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json --deep --only func_800762BC
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json func_800762BC
disc1_preflight: PASS (deep, 1658 c / 340 asm / 2 rodata)
LEAF_STRONG=EXACT func_800762BC span=0x80 vram=0x800762BC compiled=0x80 failures=[none]
```

## Load-bearing levers

Patch 3 fills the `jr $ra` slot with `addiu $sp` (2 words otherwise). `if (tw) { ... return ...; } return 0;` (body first) gives retail bnez/j layout; early `if (!tw) return 0` costs 31 words. `int code[4]` stack array reproduces the four spill stores. `(x & 0xFF) >> 3` on a short gives lbu+srl; `((-w) & 0xFF) >> 3` gives negu/andi/sra.
