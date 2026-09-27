# func_800375E0 — PARKED at 81 words (lane exeB, 2026-09-27)

- VRAM `0x800375E0`, file offset `0x27DE0`, size `0x284`; damage-popup slot allocator
  (4 × 56-byte `Slot` at `D_800BCEA8`, up to 5 numbers split into decimal digits).
- Profile tried: `era_o2_g8`.
- Best draft `build/lanes/exeB/drafts/func_800375E0.c`: **81** mismatches (was 102 in
  `build/archive/fanout/tooling2/drafts/func_800375E0.c`).
  Command: `tools/analysis/era_link_check.py src/func_800375E0.c 0x800375E0 0x284 -O2 -G8`.

## Levers that moved it
- Digit loop with the division in the condition:
  `k = 0; while (q = (short)v / 10, p[k] = v - q * 10, (short)(v = q) != 0) k++;`
  (the same lever that closed `func_800328DC`).
- Literal `-1` for both the `D_8009CEA4` store and the sentinel compare, with no `end`
  local: one hoisted `-1` in `$t6`, as in retail. A `short`/`int`/`signed char end` local
  keeps a second copy. On its own this is 157, because the register shift below remains.
- `register unsigned short *nums asm("$12") = nums0;`: retail copies `nums` out of `$a2`
  into `$t4` because the row offset `i*56` takes `$a2`. 157 → 81.

## Residual
1. Order of the entry invariants: retail has `0x66666667` in `$t5` before
   `&D_800BCEC2` in `$t7`; cc1 emits the base first (`$t5`) and the magic in `$t3`.
2. j-loop digit pointer: retail forms `(j*6 + i*56) + &D_800BCEC2` with `i*56` recomputed
   into `$t3` and the base hoisted to the function entry. cc1 hoists `(i*56 + base)` as one
   invariant (`addu $t5,$t2,$t5`). `D_800BCEA8[0].dig[j] + i*56` gives the same result.
   Integer or `char *` sums (`(int)dig + (j*6 + i*56)`, `&dig[0][0] + (...)`) fold the base
   into the addressing (153).
3. Pinning `i` (`$7`) or `x` (`$9`) adds prologue moves (155).

## Second pass (2026-09-27, exeB): structure now matches retail, register homes do not
Draft updated to `build/lanes/exeB/drafts/func_800375E0.c` (83 mismatches). 77 is reachable
with the zero test written as `(short)q != 0` and `v = q` moved into the body, but that is
structurally further from retail.
- The digit pointer is assigned inside the loop condition:
  `while (q = (short)v / 10, p = D_800BCEA8[0].dig[0] + (j * 6 + i * 56), p[k] = v - q * 10, ...)`.
  loop.c then hoists `p` out of the digit loop, and `&D_800BCEC2` out of every loop into the
  entry preheader. That gives retail's entry order `-1, 0x66666667, &D_800BCEC2` and retail's
  per-j `addu $t2,(j*6 + i*56),base`. This fixes both residual classes of the first pass.
- Left: global-alloc homes. Retail: i/v `$a3`, 1/k `$t0`, x/j `$t1`, p `$t2`, i*56 `$t3`,
  magic `$t5`, -1 `$t6`, base `$t7`, mfhi temp `$t8`. cc1: v `$a2`, k `$a3`, j `$t0`, p `$t1`,
  i*56 `$t2`, magic `$t3`, -1 `$t5`, base `$t6`. Pinning j/k/v/p to the retail registers
  shortens the object to 560 bytes (the loops restructure), so that does not work. The 36-way
  type sweep of j/k/v/q (`build/lanes/exeB/v/e2_res.txt`) shows `int` types strength-reduce
  (544 bytes); retail's `andi 0xff` needs `unsigned char` j/k.
