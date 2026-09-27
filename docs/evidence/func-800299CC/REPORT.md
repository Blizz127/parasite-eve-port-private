# func_800299CC — EXACT (lane exeD3, 2026-09-27)

- VRAM `0x800299CC`, file offset `0x1A1CC`, size `0x10CC` (1075 words). Battle main tick.
- It fills the whole former `[0x1A1CC, asm]` span up to `func_8002AA98` (`0x1B298`), so no resume row is needed.
- Profile `era_o2_g8_hud_colour_block`: `-O2 -G8`, `MASPSX_THREE_WORD_SYMBOL_STORE=1`,
  `MASPSX_HOIST_BLOCK_LI=D_8009CDDC`,
  `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D254,D_8009D230,D_8009D250,D_8009CDDC,D_8009D1F4,D_8009D1F0,D_8009D1A0,D_8009D290,D_8009D294,D_8009D1AC,D_8009D20C,D_8009D235,D_8009D298,D_800B0CE6,D_8009D29A,D_8009D29C,D_8009D2E8,D_8009D1CE`
- Source: `src/func_800299CC.c`. The draft came from exeA and exeD2. This lane added pins for colour blocks 3 and 4.

## Verification (private config build/lanes/exeD3/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD3/disc1.yaml --profiles build/lanes/exeD3/disc1_build_profiles.json --deep --only func_800299CC
  disc1_preflight: PASS (deep, 2080 c / 192 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD3/disc1.yaml --profiles build/lanes/exeD3/disc1_build_profiles.json func_800299CC
  LEAF_STRONG=EXACT func_800299CC span=0x10CC vram=0x800299CC compiled=0x10D0 failures=[none]
```
`profile_necessity.py` has no `--config` option and reads the main yaml, so it was skipped. K4 is shown to be load-bearing by the ladder below.

## Load-bearing levers (era_link_check word mismatches)
- The exeA draft with no K4: 942 mismatches, 880/1075 words aligned. Colour-block `li` placement is the only residual.
- **maspsx K4 `MASPSX_HOIST_BLOCK_LI`** (new in this lane): the exeA draft drops to 30 mismatches and the exeD2 pinned draft to 18. In each colour block, retail places the first n-2 constants before the first `D_8009CDDC` reload and the last two in the delay slots of reloads 1 and 2. cc1 instead places each constant in its own reload slot.
- Register pins in the colour blocks, 18 → 0:
  - block 2 (`& 3 == 1`): `$5/$6/$4` = 0x50/0xA3/0xBE (from exeD2)
  - block 3 (`== 2`): `$7/$8/$6/$5/$4` = 0x9F/0xFF/0xF9/0x46/0x82
  - block 4 (`== 3`): `$5/$6/$4` = 0x50/0xA3/0xBE
- Levers kept from exeA: the `char pad[0x1A0]` frame, the asm-label alias `D_8009CDDC_b` on block 4's last store, the `Ent.f18` union, and `D_8009D298/29A/29C` as `[1]` arrays.
