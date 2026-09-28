# func_8002B29C — PARKED (lane exeA, 2026-09-27)

- VRAM `0x8002B29C`, file offset `0x1BA9C`, size `0x6B0`
- Best: 198 (this lane, macro draft; linked 0x6E0) / 226 (decompA draft, linked 0x680 — 12 words short)
- Profile tried: -O2 -G8, MASPSX_THREE_WORD_SYMBOL_STORE=1, MASPSX_DISPATCH_FOLD=jtbl_80010910, MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D254,D_8009D20C,D_8009CDDC
- Best draft: `build/lanes/exeA/drafts/func_8002B29C.c` (not in `src/`)

## Residual

States 0/1 exact. States 2–4 build the `D_800BE9F0[D_8009CDDC]` POLY_FT4 quad; the
retail arithmetic is a PsyQ `setXYWH` expansion (`x1 = (100-4t) + (120+8t)` reassociated to
`(8t+220)-4t`) — the draft now uses `setXYWH`/`setRGB0` macros on `(&D_800BE9F0[D_8009CDDC])`,
which fixes the arithmetic and the per-group temp pointer, but the frame grew to `0x50` and the
constant materialisation (`li 120`/`li 122` up front) and `sltiu` vs `slti` (`unsigned t`) still
differ.

## Levers tried

block-local quad pointers (225), macro form (198), `unsigned int t` (234).

Measured with `tools/analysis/era_link_check.py <draft> 0x8002B29C 0x6B0 <flags>` (profile env in a subshell).

## Round 2 (2026-09-27)

- Dropping `char unused[24]` restores retail's 0x38 frame with the macro draft (the macros removed the spill that the pad
  was compensating for). Retyping `unsigned char t` gives the `andi`/`sltiu` shape: 341/428 aligned (`build/lanes/exeA/al.sh`).
- The remaining state-2/3 gap includes retail loading `li 120`/`li 122` at the block top, the same colour-constant
  hoist as the HUD functions (maspsx knob K4, pending). Re-test with K4 when it lands.

## Round 3 (lane exeA3, 2026-09-27) — 407/428 aligned, size exact (lnk 428), draft build/lanes/exeA3/drafts/func_8002B29C.c
- case 4 now exact: `PolyFT4 *p = Q;` taken BEFORE the r0 store (retail stores r0 and the 8 XY halves through the
  same `$2`, then g0/b0 through fresh `Q` reloads), XY stores written in setXY4 order (x0,y0,x1,y1,x2,y2,x3,y3), and
  `char pad[24]` restored (frame 0x38).
- case 2: `register int k7a asm("$5") = 0x7A` + `register int k78 asm("$4") = 0x78` (c = k78 - 4t) fix the prefix;
  explicit XY stores with constant 0x7C for y2/y3.
Residual (case 2/3): `li 122`, `li 100` and the `la D_800BE9F0` land one group later/earlier than retail (sched),
plus `$4/$5` swaps in case 3's then-arm. K4 does not apply (single constant per reload).

## Round 4 (lane exeA4, 2026-09-27) — unchanged 407/428
Case 2 residual is sched1 placement: retail puts `li 122` at the block top, `la D_800BE9F0` before the third `sb`
(so the fourth `D_8009CDDC` reload uses `$4`) and `li 100` late; cc1 fills the third reload's load-delay with `li 122`.
K4 does not apply (only one constant follows a group load). `k7a` unpinned / assigned later / `t` read earlier are
byte-identical.
