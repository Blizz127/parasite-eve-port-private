# func_800829C4

- VRAM `0x800829C4`, size `0x118` (70 words), file offset `0x731C4`
- Profile: `era_o2_g0_fill_epilogue_delay_slot`, flags `-O2 -G0`, env `MASPSX_FILL_EPILOGUE_DELAY_SLOT=1`
- Source: `src/func_800829C4.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_800829C4
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_800829C4
  -> LEAF_STRONG=EXACT func_800829C4 span=0x118 failures=[none]
(MASPSX_FILL_EPILOGUE_DELAY_SLOT=1 python3 tools/analysis/era_link_check.py src/func_800829C4.c 0x800829C4 0x118 -O2 -G0)
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

Early 'if (cur == a0) return cur;' (the nested-if form gives 13-15 words); the two timeout counters go through pointer locals p = &D_800A5AC0 / q = &D_800A5AC4 so retail's callee-saved base registers ($s2, then $s0 reused) carry the load, the call and the zero store; patch 3 epilogue fill.
