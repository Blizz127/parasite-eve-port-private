# func_80082E00

- VRAM `0x80082E00`, size `0x214` (133 words), file offset `0x73600`
- Profile: `era_o2_g0_fill_branch_macro_split_epilogue`, flags `-O2 -G0`, env `MASPSX_FILL_BRANCH_MACRO_SPLIT=1 MASPSX_FILL_EPILOGUE_DELAY_SLOT=1`
- Source: `src/func_80082E00.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_80082E00
  -> disc1_preflight: PASS (deep, 1666 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_80082E00
  -> LEAF_STRONG=EXACT
(MASPSX_FILL_BRANCH_MACRO_SPLIT=1 MASPSX_FILL_EPILOGUE_DELAY_SLOT=1 python3 tools/analysis/era_link_check.py src/func_80082E00.c 0x80082E00 0x214 -O2 -G0)
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

drain loop uses its own pointer `lq` built from a `$3`-pinned scaled index `(D_8009B764 << 2) + tbl` (index-first operand order); exit re-read spelled `*(D_8009B764 + tbl)`; loading the handler into `f` before `*q = -1` lets reorg put the store in the jalr slot; patches 10 + 3.
