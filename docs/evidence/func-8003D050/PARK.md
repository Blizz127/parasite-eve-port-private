# func_8003D050 — PARKED at 37 words (lane exeB3, 2026-09-27)

- VRAM `0x8003D050`, file offset `0x2D850`, size `0x7DC` (model/primitive binder: part and
  vertex table pointers into `Obj`, GT4/G4/GT3/G3 code bytes, UV copy, part-index table, identity
  matrices, then `func_8003D94C` for the palette rebase).
- Profile `-O2 -G8` + `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009CDDC` (existing
  `era_o2_g8_force_d8009cddc_absolute`).
- Best draft `build/lanes/exeB3/drafts/func_8003D050.c`: **37** word mismatches (decompA draft: 464).
  Command: `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009CDDC tools/analysis/era_link_check.py src/func_8003D050.c 0x8003D050 0x7DC -O2 -G8`.
  Every register home matches retail. The residual is all in the **entry block schedule**
  (the first ~45 words) plus the trailing alignment word.

## Levers found (ladder)
1. `int flag` (retail `lw t8,92(sp)`, `beqz t8`; a `short` flag gives `sll/sra` tests).
2. **`short n`**, initialised at the top: the loop-entry tests fold to `beqz` (an `int n = 0`
   gets cse'd in as the `slt` operand for `i = 0`).
3. f18 rounding: `{ int k = m->f18 >> 2; short r = m->f18 & 3; if (r > 0) c += (k + 1) * 4; else c += k * 4; }`
   gives retail's shared `srl`, `blez`, and both-arm `sll`. The inline `(f18 & 3) > 0` form becomes `beq`,
   and one expression folds to `andi 0xfffc`.
4. `signed char f8C` (retail `li -1`).
5. **Parameter is the moving pointer**: `unsigned char *c` is the parameter and `m = (Model *)c`
   a copy (`o->f0 = (Model *)c; m = (Model *)c; c += 0x1C;`). `c` then gets `a1` and `m` a
   temporary, as in retail.
6. Post-call part: a fresh pointer `r = p;` after `o->f70`/`o->f72`; parts-loop stores in the order
   `+8, +10, +6` (gives the `r+6` giv base).
7. Second flag block: a per-outer-iteration copy `s = c;`, inner loop reads `s[...]`.
8. **Increments in the for-clause**: `for (...; i++, f += 12)` for the four code-byte loops and
   `for (...; i++, r += 0x20)` for the matrix loop (retail increments the biv after `i`).
9. **`asm("" : "=r"(p) : "0"(p));` before `o->f54 = p;`** (485 → 64). Without it, loop.c takes `a2`
   (the incoming register) as the biv's initial value, so the `p+7` giv prefers `a2` and the whole
   allocation cascades. After it, the homes match. A clobber `asm("" ::: "$6")` at the head of the
   flag block also fixes the giv but perturbs the other homes (484).

## Residual (37)
Retail's entry schedule: `i = 0` (`t2`) in the `lbu nParts` load slot, `n = 0` in the first `lhu nVerts`
slot, the `p` copy `move s1,a2` in the second `lhu nVerts` slot just before `sw s1,84(s0)`, and
`lw s5,88(sp)` after `sw f10`. Ours: the barrier pins the `p` copy to the top, and `i = 0` for loop 1
sits in the `beq` delay slot inside `if (flag)`. Tried: 240 combinations of barrier position ×
`i = 0` position before the `if` (all ≥ 37; any `i = 0` in the entry block swaps the `a6`/`a7`
homes, `t9`/`a2` against retail's, because `a7` wins the priority tie, giving 500); a `w = p` copy for
the loops (481–486); clobber variants (198–498).
Next idea: find a label or other loop.c `record_initial` stop between the `p` copy and loop 1 that
emits no code, so the tie barrier can go.

## Fifth pass (lane exeB4, 2026-09-27): 37 → 28
Best draft `build/lanes/exeB4/drafts/func_8003D050.c` (**28** word mismatches, same command/profile).
- **Split the parameter**: rename the incoming pointer `p0` and tie it into a fresh local,
  `asm("" : "=r"(p) : "0"(p0));` (instead of the self-tie on `p`). Homes still all match; the copy
  now lands at +0x30 instead of +0x0C. The tie position anywhere from the top to just before
  `o->f54 = p` gives the same 28; after `o->f54` it is 184.
- A **plain copy `p = p0;`** (no asm, `drafts/func_8003D050_plaincopy.c`) gives the same entry
  schedule and the same homes (the word count reads 481 only because `i = 0` moves into a nop slot and
  shifts every later word by 4; the aligned diff is 21 lines against e0's 23). So the loop.c
  initial-value problem is gone once the copy is from a pseudo, not from the incoming `$6`.
- Residual (both forms): sched1 hoists the stack-param loads (`out`, `a4..a7`, RTL insns at entry,
  no memory dependence on the `o->` stores) above the second `lhu nVerts` and the `p` copy, so `$6`
  (p0) is still live when `a7` is loaded and `a7` cannot take `$6`; retail has them after `sw f10`.
  Also `i = 0` sits in the loop-1 `beq` slot; any `i = 0` in the entry block (17 positions) swaps
  a6/a7 homes (491) and self-ties on a6/a7/flag only move the swap to flag/a6 (30–312).
- Source order `c += m->nVerts * 4;` **before** `o->f54 = p;` (`drafts/func_8003D050_r1_copyslot.c`,
  plus an `o` self-tie at the top to restore the o/p = s0/s1 homes) puts the copy and `sw f54`
  exactly where retail has them (right after the second `lhu`). But sched1 still puts the a4..a7 loads
  first, so every temporary register below `$16` shifts by one (180 words, 327 aligned diff lines).
- Tried, all worse: `asm volatile` tie (490–497), tie with extra `"m"`/`"r"` inputs to order it
  (478–490), a `t = m->nVerts` shared load (490), `-fno-schedule-insns` (401–501),
  `-fno-schedule-insns2` (47–442), the cse1 label boundary `L: z = 0; if (z) goto L;` at every
  entry position (187; the label is gone before loop.c runs).
Next idea: find what gives the stack-param loads a dependence on the `o->` stores in retail's sched1
(so they are placed after `sw f10`). Possibly the params were read through a pointer, or declared
differently (`volatile`, address taken, or a struct passed on the stack).

## Sixth pass (lane exeB5, 2026-09-28): no change (28)
Aligned diff of the exeB4 draft is 12 lines, all in the entry block: retail places the `lw 88` / `lhu 72..84`
stack-parameter loads after `sw f10` (right before `beq flag`), ours between `sw fC` and `sh flag`.
Tried making the stack params addressable (dead `if (0) f(&aN)` for a7 / a4..a7 / out / flag / a6 / a6+a7 and
all together) to turn them into use-site stack loads: 21–257 aligned lines, all worse.
