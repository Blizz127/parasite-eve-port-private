# func_80029810

- VRAM `0x80029810`, file offset `0x1A010`, size `0x1BC` (111 words)
- Profile: `era_o2_g8_force_d8009d254_plus7_battle_reset_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D254,D_8009D290,D_8009D1AC,D_8009D1A8,D_8009D1CE,D_8009D235,D_8009D250,D_8009D27C (new profile)
- Source: `src/func_80029810.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80029810
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80029810
LEAF_STRONG=EXACT func_80029810 span=0x1BC vram=0x80029810
```

## Load-bearing levers

Battle-state reset. `-G8` keeps eight gp scalars (including `D_8009D278` at 0x508 here), with eight absolute symbols forced. Levers: (1) the byte clear `*(unsigned char *)&D_8009D1AC = 0` precedes the word RMW on the same address (retail does not forward); (2) a `register unsigned int fl asm("$3")` pin on the RMW load puts the value in `$v1` and the `~0x300` mask in `$v0` (5 words otherwise; int typing, union/struct and statement moves do not help); (3) plain `D_800B8A90[i] = 0` (cc1 hoists that base itself) while the `D_800A7FF0[i].a/.b` pair stays symbol+index; (4) `unsigned char id` parameter, `unsigned char i` counters.
