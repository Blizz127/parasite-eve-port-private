# func_80061044 — LINK_EXACT (lane exeC, 2026-09-27)

- VRAM `0x80061044`, file offset `0x51844`, size `0x45C` (ends on the `jr $ra` before `func_800614A0` at `0x51CA0`).
- yaml: insert `- [0x51844, c, func_80061044]` after `- [0x50E2C, asm]` (that asm span now holds only
  `func_8006062C`, `0x50E2C + 0xA18 = 0x51844`) and before `- [0x51CA0, c, func_800614A0]`.
- Profile `era_o2_g8_expand_div` (existing: `-O2 -G8`, `MASPSX_EXPAND_DIV=1`).
- HUD gauge: two POLY_G4 bars (width `min(a0,a1)*56/a1` with div guards), a TILE and a DR_MODE from the
  `D_8009D100` packet pool, each linked into `*D_8009D11C`.

## Verification (private lane config `build/lanes/exeC/`)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json --deep --only func_80061044
  disc1_preflight: PASS (deep, 2046 c / 214 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json func_80061044
  LEAF_STRONG=EXACT func_80061044 span=0x45C vram=0x80061044 compiled=0x460 failures=[none]
```
Necessity (manual): without `MASPSX_EXPAND_DIV` → 226 words; `-G0` (+div) → 266 + size change. Both knobs load-bearing.

## Levers (ladder)

decompB draft: 49 words (retail `a1`→`$s0`, pool pointer→`$s1` shared with the width `w`; cc1 swapped them).
1. **One variable for the first bar's pool pointer and the width**: `w` (int) holds the allocated packet
   (`w = (int)(D_8009D100 - 9)`, `p = (PolyG4 *)w`) and is then reassigned the width. Retail's `$s1` holds both
   (`mflo $s1`); one pseudo gives it the right global-alloc priority → 32.
2. **Store-order search** (greedy single-line moves, `build/lanes/exeC/lsearch.py`): bar 1 needs
   `r0/r2 = 0; y0/y1 = dy + 170; x1/x3 = w + 229;` in that order → 23 → 20.
3. **Tag link split around the colour globals**: `((Tag *)p)->addr = ((Tag *)D_8009D11C)->addr;` *before*
   `D_8009D110 = …; D_8009D114 = …; D_8009D10C = 0;` and the `D_8009D11C` update after them — in bar 2 (→ 5) and
   likewise in the DR_MODE tail (→ 0). This fixes the `0xFFFFFF` mask materialization order/register.
The PSY-Q `addPrim` macro spelling instead of the bitfield `Tag` is much worse (173–216).
