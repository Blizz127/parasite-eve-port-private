# func_80015C7C

- VRAM `0x80015C7C`, file offset `0x647C`, size `0x130` (76 words)
- Profile: `era_o2_g8_force_d8009d1a0_d8009d2a4_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D1A0,D_8009D2A4 (new profile)
- Source: `src/func_80015C7C.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80015C7C
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80015C7C
LEAF_STRONG=EXACT func_80015C7C span=0x130 vram=0x80015C7C
```

## Load-bearing levers

Gate-family sibling: `-G8` for `D_8009D300`/`D_8009CE00`, `D_8009D1A0` and the short `D_8009D2A4` forced absolute, `D_800B0CD8` as an unsized array (so the address stays in `$v1` and the `sw` lands in the `jal` delay slot). The fifth context word is passed as a pointer (`a0[4]`, not dereferenced) to `func_8005D2B4`.
