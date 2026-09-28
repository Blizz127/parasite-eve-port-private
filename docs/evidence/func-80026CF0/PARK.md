# func_80026CF0 — PARKED (lane exeA, 2026-09-27)

- VRAM `0x80026CF0`, file offset `0x174F0`, size `0x2E0`
- Best: 21 word mismatches (from 153), size exact
- Profile tried: -O2 -G8, MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D278
- Best draft: `build/lanes/exeA/drafts/func_80026CF0.c` (not in `src/`)

## Residual

Only the two type-1 group-skip loops (A with `D_8009D2D8++`, B without) remain. Retail
bodies are `lbu; addiu -1; sb; andi $v0,$v0,0xFF` (the `(u8)` of the stored decrement taken
*after* the store, into the same register), and B's entry is cross-jumped into the body's
`sb` (`j body_sb` with `n-1` in the delay slot) while the preceding `bne` slot is filled from
A's fallthrough `addiu $v1`. Every spelling that masks puts `andi` before `sb` (sched1), and a
volatile-asm barrier between them breaks the B-entry cross-jump.

## Levers tried

landed: goto-into-test loops (`goto t; b: ...; t: if (...) goto b;` = retail's
non-rotated `j test` form), test on an `int c` assignment value, loop C as the reload form
(`store; t: if (D_CC && ...) { D_CC--; goto t; }`), `(int)` cast for the `((f10&0xF)*3)>>1`
`sra`, `(&D_800BE830[i])->grp = 0` pointer form in the clear loop, `register int c asm("$2")`.
Tried: `while`/`if + do-while` forms (no cross-jump, 154–164), `unsigned char`/`unsigned int`
c, `D_CC = c = ...` vs `c = D_CC = ...`, `c = --D_CC`, asm barriers (132–164).

Measured with `tools/analysis/era_link_check.py <draft> 0x80026CF0 0x2E0 <flags>` (profile env in a subshell).

## Round 2 (2026-09-27)

Tried on the 21-word best draft (`w1`) and its variants: `-fno-schedule-insns` (151), `-fno-schedule-insns2`
(30). None help. The residual is still the body `addiu v0; sb v0; andi v0,v0,0xFF` order (sched1 puts the `andi` before the
`sb`). The asm-label alias lever does not apply here because the problem is a missing cross-jump, not an unwanted one.
- Size-checked `build/lanes/exeA/lsearch.py` over the 27-line loop head: start 21, final 21 (no statement move helps).

## Round 3 (lane exeA3, 2026-09-27)
Best unchanged (exeA2 draft 176/184 aligned). Findings: with c unpinned and B's entry written as
`c = n - 1; goto s2; b2: c = D - 1; s2: D = c; c &= 0xFF;` the loop bodies are right (andi after sb) but c gets `$4`.
With c pinned `$2` the bodies are right but reorg steals B's `addiu $2,$5,-1` into the `bne` slot and redirects to the
`sb` (retail fills that slot from A's fallthrough `addiu $3,$5,-1`); per agbcc-era reorg.c this means retail's
mostly_true_jump predicted not-taken or the target steal was blocked. Block-local pins for A/B, entry reorderings,
`-fno-{thread-jumps,cse-follow-jumps,cse-skip-blocks,expensive-optimizations,schedule-insns2}` do not help.

## Round 4 (lane exeA4, 2026-09-27) — unchanged 176/184
Mechanism narrowed: retail fills the `bne` slot from A's fall-through (`addiu $3,$5,-1`) in `fill_simple_delay_slots`
("insns beyond the branch", needs the insn not to set a reg live at B). In the pinned-`c` draft A's `n-1` lands in `$4`
and `D_8009D2D8`'s temp in `$3` (retail `$3`/`$2`: `c` must not be live across `D_8009D2D8++`), so the fall-through fill
fails and `fill_eager_delay_slots` (NE ⇒ predicted taken) steals B's entry. Tried `x = n-1; D = x; D_2D8++; c = x & 0xFF`
(n-1 gets CSE'd from the top index, 174), `unsigned char x` / `(unsigned char)(n-1)` after the increment (176),
`register int x asm("$3")` (copy `addu $3,$4`, 176).
