# func_80064B74 — matched (lane 4, session 2026-09-22)

- VRAM `0x80064B74`, size `0xAC`, source `src/func_80064B74.c`.
- Profile: `era_o2_g8` (-O2 -G8).

## Verification (private lane config)

```text
python3 tools/build/disc1_preflight.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json --deep --only func_80064B74
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json func_80064B74
disc1_preflight: PASS (deep, 1666 c / 337 asm / 2 rodata)
LEAF_STRONG=EXACT func_80064B74 span=0xAC vram=0x80064B74 compiled=0xB0 failures=[none]
```

## Load-bearing levers

`-G8` for the gp-relative D_8009D16C (0x3FC($gp)); `o->f70 = idx` written before `if (o == 0 || idx < 0) return` (retail stores it in the beqz delay slot); `signed char D_800A3060[][4]` rows for lb. First attempt LINK_EXACT.
