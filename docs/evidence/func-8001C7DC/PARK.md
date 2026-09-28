# func_8001C7DC — PARKED (lane exeA, 2026-09-27)

- VRAM `0x8001C7DC`, file offset `0xCFDC`, size `0x2D4`
- Best: 125 (decompA draft) / 128 (this lane)
- Profile tried: -O2 -G8, MASPSX_EXPAND_DIV=1, MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2E8
- Best draft: `build/lanes/exeA/drafts/func_8001C7DC.c` (not in `src/`)

## Residual

Sibling of the now-exact `func_8001CBA0` (same projection + perpendicular-distance +
axis-nudge loop, segment endpoints read from `$gp` globals `D_8009CE0C..12`). Retail loads
`D_8009CE0C` with `lhu` + `sll 16`/`sra 16`, keeps `x0<<16` (`$t0`) and `x1` (`$a3`) live from
the first distance test into the `k` loop, and reloads `z0/z1`, the wall index and the wall
pointer per outer iteration. cc1 folds the `lhu/sll/sra` into `lh` and allocates the callee
saves differently (`o` `$s4`/`$s5` swap).

## Levers tried

the `func_8001CBA0` levers (two-arm load, px/pz add placement); `unsigned short x0` +
`(short)x0` casts; `int x0 = (short)D`; `x0 = D << 16` with `>> 16` uses; `short x1`.

Measured with `tools/analysis/era_link_check.py <draft> 0x8001C7DC 0x2D4 <flags>` (profile env in a subshell).

## Round 2 (lane exeA4, 2026-09-27) — best still the exeA draft (108/181 aligned via build/lanes/exeA4/al.sh)
Finding: retail's outer `k` loop is **not a loop.c loop** — `D_8009CE0E`, `D_8009CE12`, `D_8009CE18`, `D_8009CE14`
and `(short)x0` are re-read at the top of every `k` iteration (hoisted only out of the inner `j` loop), while cc1's
`for (k = 0;; k++)` hoists them out of `k` as well (preheader copies `$10=$4`, `$7=$5`). Writing the outer loop as
`k = 0; kloop: { for (j...) {...} k++; goto kloop; }` reproduces the per-`k` reloads
(`build/lanes/exeA4/drafts/func_8001C7DC_gotoloop.c`, 93/181 because the loop body order still differs:
retail hoists `dx = x1 - (short)x0` to k-level but keeps the wall address `D_8009CE14 + D_8009CE18*12` in the j loop;
cc1 does the opposite). Retail also keeps `x0 << 16` (`$8`) live from the first distance into the loop (`$4 = $8`,
`sra $11,$4,16` per k) and copies `x1` (`$3 = $7`) in the preheader. Next: shape the j-loop expression so loop.c
hoists dx but not the wall address (e.g. wall pointer read through a volatile/alias, or `len` loaded per j).

## Round 3 (lane exeA5, 2026-09-28) — unchanged 108/181 aligned
- `-dL` dump (`cc1 -O2 -G8 -dL`) of the `for (k;;)` draft shows loop.c sees **only one loop** (the `j` loop; the outer
  infinite `k` loop is not processed), so every invariant lands in the `j` preheader = once per `k`. That matches
  retail's per-`k` reloads; the goto `kloop` rewrite is not needed for that.
- The remaining body-order difference is loop.c's movable selection: cc1 moves the wall-address chain
  (`D_8009CE18*12 + D_8009CE14`, insns "cond forces … savings 2 moved") but rejects `dx = x1 - x0s`
  ("savings 1, life 1, not desirable"); retail does the opposite. Hoisting `dx`/`dz` into named locals assigned at the
  top of the `j` body (before the switch) makes loop.c move `dx` (life 43) — draft
  `build/lanes/exeA5/drafts/func_8001C7DC_dxhoist.c` — but the address chain is still moved, so the score is unchanged.
- `int x0 = D_8009CE0C << 16` with `x0 >> 16` uses reproduces retail's `sll $8` / per-`k` `sra` shape (xg1 variant).
- Next: make the address chain "not desirable" (lifetime 1, e.g. index/base read into locals right at the use, or
  the address formed through a different expression so its savings drop to 1), then re-check the found-block layout
  (retail keeps the in-loop found block `sw $17,40; j end; sw $18,48` before the loop) and the first-distance operand
  order (retail `subu x1-x0s` before `(pz>>16)-z0`).
