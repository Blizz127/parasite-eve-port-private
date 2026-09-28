# func_8018F958 (ovl_03D2, 0x268, blend blit) — PARKED (ovl12 update, 2026-09-28)

Earlier: ovl6 94w, ovl10 83w (`build/lanes/ovl10/drafts/park_func_8018F958.c`).
ovl12 best: **58 word mismatches** — `build/lanes/ovl12/drafts/park_func_8018F958.c` (0x270 object,
2 words long); length-exact variant at 82w: `park_func_8018F958_lenexact.c`.

New structural findings (pass 2, the reverse blit):
* The inner-loop bound is a **ternary over comparisons**, not a ternary value:
  `for (j = 0; (w - n) < 0x19 ? j < (w - n) : j < 0x18; j++)` — gives retail's
  `slti/beqz → [slti j,24 in slot] … FAD0: beqz exit` layout with the else-arm block placed before
  the body (the goto-rotated do/while with `j < min()` materialises a `li 24`).
* `rows`/`w`: retail's `w` is a biv initialised **after** the `blez rows` guard (`addiu $t2,$0,84`
  in the preheader) and the bound is `w - n` recomputed per test. The 58w draft uses
  `for (i = 0; i < 0x54 - n; i++)` + bound `(0x54 - i - n)`, which loop.c reduces to one giv
  `rows - i` (retail keeps `w - n`); the length-exact draft keeps `w` but inits it before the guard.
* Remaining: pass-2 element loads — cse rewrites `*dst` as `1(p2)` after the first element
  (`lbu $8,1($6)`), retail loads `0(dst)` with `p2` a fresh copy each time (pass 1 does match this
  shape); `stride*4`/rows-copy/prologue `lw` order and $t-register rotation.
Tried: body spellings (`c=*dst;p2=dst++` / `p2=dst++;c=*p2` / `p2=dst;c=*dst++` / load-first) —
identical after cse; `d` instead of the `dst` param; `*q++` walks (worse); hand-rotated
`if (rows > 0) do … while (++i < rows)` outer (86w); `-fno-schedule-insns` not tried here.
