# func_8006DCE4 — matched (lane 4, session 2026-09-22)

- VRAM `0x8006DCE4`, size `0x54`, source `src/func_8006DCE4.c`.
- Profile: `era_o2_g0_no_expensive_optimizations` (-O2 -G0 -fno-expensive-optimizations).

## Verification (private lane config)

```text
python3 tools/build/disc1_preflight.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json --deep --only func_8006DCE4
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json func_8006DCE4
disc1_preflight: PASS (deep, 1656 c / 341 asm / 2 rodata)
LEAF_STRONG=EXACT func_8006DCE4 span=0x54 vram=0x8006DCE4 compiled=0x60 failures=[none]
```

## Load-bearing levers

`-fno-expensive-optimizations` (without it cc1 routes both a0 and a1 through temporaries: 20 words differ). Trailing three parameters declared `short` so the incoming sll/sra conversions appear at entry; callee declared unprototyped. Twin of func_8006DE80/func_8006DCE4 (resolves the prior PARK of func_8006DE80, whose PARK.md tried only spelling/-O1/sched2 rungs).
