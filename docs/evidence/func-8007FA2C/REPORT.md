# func_8007FA2C

- VRAM `0x8007FA2C`, size `0xD8` (54 words), file offset `0x7022C`
- Profile: `era_o2_g0_cc1_281`, flags `-O2 -G0 -mno-split-addresses`, env `ERA_CC1_VER=2.8.1`
- Source: `src/func_8007FA2C.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_8007FA2C
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_8007FA2C
  -> LEAF_STRONG=EXACT func_8007FA2C span=0xD8 failures=[none]
(ERA_CC1_VER=2.8.1 python3 tools/analysis/era_link_check.py src/func_8007FA2C.c 0x8007FA2C 0xD8 -O2 -G0 -mno-split-addresses)
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

Pinned $3 counter / $2 base (retail's allocation); the first byte walk uses a block-scoped pointer q = p + 8 (a function-scope or pinned q keeps $a0 busy and hoists the func_80080B44 argument); the second walk reuses p itself (p += 0xB; *p-- = 0) as retail does; both loops are do/while with --i >= 0.
