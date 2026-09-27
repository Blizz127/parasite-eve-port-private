# func_80077144 — matched (lane 4, session 2026-09-22)

- VRAM `0x80077144`, size `0x150`, source `src/func_80077144.c`.
- Profile: `era_o2_g0_cc1_281` (-O2 -G0 -mno-split-addresses, env ERA_CC1_VER=2.8.1).

## Verification (private lane config)

```text
python3 tools/build/disc1_preflight.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json --deep --only func_80077144
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json func_80077144
disc1_preflight: PASS (deep, 1669 c / 338 asm / 2 rodata)
LEAF_STRONG=EXACT func_80077144 span=0x150 vram=0x80077144 compiled=0x150 failures=[none]
```

## Load-bearing levers

Band compiler 2.8.1. `switch (mode & 7)` with case 0/5 and 1/3 groups; non-volatile hardware pointer globals so cc1 fills the jal slot with `sw $zero`; volatile D_80095874/78 so `D_80095874 = D_80095878` reloads after the zero store. `if (mode & 7) return 0; return func_80077548(mode);`.
