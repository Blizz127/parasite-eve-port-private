# func_8007A2A4

- VRAM `0x8007A2A4`, size `0x80` (32 words), file offset `0x6AAA4`
- Profile: `era_o2_g0_load_delay_call_slot_store`, flags `-O2 -G0`, env `MASPSX_LOAD_DELAY_CALL_SLOT_STORE=1`
- Source: `src/func_8007A2A4.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_8007A2A4
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_8007A2A4
  -> LEAF_STRONG=EXACT func_8007A2A4 span=0x80 failures=[none]
(MASPSX_LOAD_DELAY_CALL_SLOT_STORE=1 python3 tools/analysis/era_link_check.py src/func_8007A2A4.c 0x8007A2A4 0x80 -O2 -G0)
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

Plain C (D_8009AFD8 == 1 dispatch, then two byte clears through pointer globals); the final sb lands in the jal slot after the load-delay nop only with maspsx patch 13 (MASPSX_LOAD_DELAY_CALL_SLOT_STORE=1; 6 words without). 2.7.2, not 2.8.1 (2.8.1 + patch 13 misses by 2).
