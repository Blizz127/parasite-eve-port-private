# func_8002BC90 — matching C leaf (PARK resolved)

- VRAM `0x8002BC90`, file offset `0x1C490`, size `0x1560` (1368 words)
- Source: `src/func_8002BC90.c` (draft by lanes exeA/exeA3, `build/lanes/exeA3/drafts/`)
- Profile: `era_o2_g8_hud_colour_block_tail1` = `{'toolchain': 'era', 'flags': ['-O2', '-G8'], 'environment': {'MASPSX_THREE_WORD_SYMBOL_STORE': '1', 'MASPSX_FORCE_ABSOLUTE_SYMBOLS': 'D_8009D254,D_8009D230,D_8009D250,D_8009CDDC,D_8009D1F4,D_8009D1F0,D_8009D1A0,D_8009D290,D_8009D294,D_8009D1AC,D_8009D20C,D_8009D235,D_8009D298,D_800B0CE6,D_8009D29A,D_8009D29C,D_8009D2E8,D_8009D1CE', 'MASPSX_HOIST_BLOCK_LI': 'D_8009CDDC', 'MASPSX_HOIST_BLOCK_LI_TAIL1': 'D_800B0130'}}`

## Verification (main config, 2026-09-27)

```
python3 tools/analysis/leaf_strong_check.py func_8002BC90   -> LEAF_STRONG=EXACT span=0x1560 failures=[none]
python3 tools/build/disc1_preflight.py --deep --only func_8002BC90   -> PASS (deep, 2101 c / 179 asm / 2 rodata)
python3 tools/analysis/profile_necessity.py --only func_8002BC90     -> PROFILE_NECESSITY=PASS
```

## How it closed

One of the three HUD-tick functions whose colour blocks load every colour constant at the block
head, ~3 store groups before first use; cc1 places each `li` right before its first store.
maspsx **K4 `MASPSX_HOIST_BLOCK_LI=D_8009CDDC`** hoists them; the `D_800B0130` gauge blocks put
constants 0..n-2 before the first index reload and the last one after it, which is
**`MASPSX_HOIST_BLOCK_LI_TAIL1=D_800B0130`** (lane exeD4; test
`tools/era/maspsx/tests/test_hoist_block_li_tail1.py`, flag-off sweep 4612/4612 identical).
C-side levers (exeA3, see PARK.md history): asm-label alias on the last store of the identical colour
block (stops cc1 merging the blocks), sized `[1]` arrays for `D_8009D298/29A/29C` and
`D_8009CE34` (in-struct alias class), unions for overlapping halves, per-block
`register int k asm("$N") = const` pins for swapped colour constants, per-site `*fl` vs
`((W1*)fl)->v` to steer the gcc 2.7.2 sched in-struct alias heuristic.
