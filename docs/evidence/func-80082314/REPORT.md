# func_80082314

- VRAM `0x80082314`, size `0xEC` (59 words), file offset `0x72B14`
- Profile: `era_o2_g0_fill_epilogue_delay_slot`, flags `-O2 -G0`, env `MASPSX_FILL_EPILOGUE_DELAY_SLOT=1`
- Source: `src/func_80082314.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_80082314
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_80082314
  -> LEAF_STRONG=EXACT func_80082314 span=0xEC failures=[none]
(MASPSX_FILL_EPILOGUE_DELAY_SLOT=1 python3 tools/analysis/era_link_check.py src/func_80082314.c 0x80082314 0xEC -O2 -G0)
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

D_800B28F8 is volatile (spin loop re-reads it and the return re-reads it once more); callbacks declared with exact arity; the addiu $sp epilogue fill needs maspsx patch 3 (2 words without it).
