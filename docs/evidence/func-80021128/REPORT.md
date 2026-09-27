# func_80021128

- VRAM `0x80021128`, file offset `0x11928`, size `0x150` (84 words)
- Profile: `era_o2_g8_force_d8009d278_d8009d254_d8009d2fc_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D278,D_8009D254,D_8009D2FC (new profile)
- Source: `src/func_80021128.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80021128
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80021128
LEAF_STRONG=EXACT func_80021128 span=0x150 vram=0x80021128
```

## Load-bearing levers

Three levers: (1) the head record pointer is pinned `register Obj *o asm("$4")` (retail keeps it in `$a0`, and that stops `li $a0,6` being hoisted into the `bne` delay slot); (2) the post-call reload goes into a **separate unpinned** block local `q` (lands in `$v1`); (3) the 1-vs-2 selector is a ternary over **whole calls** (`cond ? F(1, D254) : F(2, D254)`), so each arm reloads `D_8009D254` and cc1 cross-jumps the shared `jal`. A ternary inside the argument list costs 31 words; if/else costs 38.
