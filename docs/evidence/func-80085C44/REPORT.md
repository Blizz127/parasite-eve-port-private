# func_80085C44

- VRAM `0x80085C44`, size `0x13C` (79 words), file offset `0x76444`
- Profile: `era_o2_g0_fill_call_delay_slot`, flags `-O2 -G0`, env `MASPSX_FILL_CALL_DELAY_SLOT=1`
- Source: `src/func_80085C44.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_80085C44
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_80085C44
  -> LEAF_STRONG=EXACT func_80085C44 span=0x13C failures=[none]
(MASPSX_FILL_CALL_DELAY_SLOT=1 python3 tools/analysis/era_link_check.py src/func_80085C44.c 0x80085C44 0x13C -O2 -G0)
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

Pointee-volatile SPU register base read through the global each time (no local), which keeps retail's per-iteration pointer reload; unsigned counter gives sltiu 0xF01; the error-string la is split across the 'j' into its delay slot by maspsx patch 8 (57 words without). 2.7.2: the reorg 'addiu -1' fixup after the stolen i++ is retail's too.
