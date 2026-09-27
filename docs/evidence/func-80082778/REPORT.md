# func_80082778

- VRAM `0x80082778`, size `0xD4` (53 words), file offset `0x72F78`
- Profile: `era_o2_g0_dispatch_8001206c_cc1_281`, flags `-O2 -G0 -mno-split-addresses`, env `ERA_CC1_VER=2.8.1 MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_DISPATCH_FOLD=jtbl_8001206C`
- Source: `src/func_80082778.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_80082778
  -> disc1_preflight: PASS (deep, 1666 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_80082778
  -> BLOCKED in the shared tree: asm/disc1/data/800.rodata.s currently holds jtbl_8001206C as label words (the tree was re-split while this span was asm), so strip_dispatch_rodata raises "pool block has no literal words". The strong check must be re-run after the orchestrator re-splits with this carve. era_link_check (below) is LINK_EXACT.
(ERA_CC1_VER=2.8.1 MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_DISPATCH_FOLD=jtbl_8001206C python3 tools/analysis/era_link_check.py src/func_80082778.c 0x80082778 0xD4 -O2 -G0 -mno-split-addresses)
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

switch on a2-1 over five byte columns; the jump table folds to the retail pool table jtbl_8001206C (MASPSX_DISPATCH_FOLD) and the indexed table load needs the three-word `$at` form.
