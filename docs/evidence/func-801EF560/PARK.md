# func_801EF560 (ovl_03C9, 0x418) — PARKED (ovl12, 2026-09-28)

Best: **12 word mismatches** — `build/lanes/ovl12/drafts/park_func_801EF560.c` (= `_vk` variant:
ovl10's 13w draft + case-0 `r.x, r.y, r.w, r.h` order and the 0x20 constant held in `k`
(`k = 0x20; D_800F3368 = k; …; k = D_800F336C;`)). Mode 1 and the mode-2 call tails are exact.

Residual (both mode-2 cases, same shape): retail
`li $v1,0x20|0x40` / `r.x` / `r.y` / `lh step` / (`lhu D_800F336C` / `li $v0,2`) / 3×`sh $v1,G` /
`li $v1,4` / `sh r.h` / `sh D_800F336A`.

Findings (cc1 `-dS -dR` dumps; the sched traces print ready lists, priorities, `7f000001` =
birthing boost):
* Frame (`$fp`) stores never depend on `D_800F33xx` stores; `o->step` loads depend on the frame
  stores and the global stores anti-depend on that load. Statement permutation of the block alone
  (2520 orders, struct-typed control block) never beats 13.
* sched2 keeps sched1's order among ties (picks the latest LUID first) except for load-hazard
  "launching"; so the retail order must already come out of sched1.
* The retail `li 0x20` at the block head requires a non-birthing, LUID-early set (a multi-set
  pseudo like `k` gets it to the top — 12w — but then allocates `$a1`, and the `lhu k` must then
  follow the stores). With the pinned `four asm("$3")` for 0x20 the stores/`li 4` order is right
  but sched1 keeps `lh step` above the constant (birthing `lh`).
* `D_800F336A = 2` is a birthing `li` in cc1 and lands right before its store; retail has it
  before the three stores (non-birthing / earlier LUID). A multi-set local for it (`t`, `c`)
  crosses calls and adds a saved register.
* Scalar vs struct typing of the five control-block globals: only `D_800F3368` matters (struct
  needed); all 32 combinations tried.
Tried and rejected: literal constants (li 4 sinks to the bne), unpinned `four` (38w), per-case
constant carriers `x/c/cy/t/k2/cy2/g/gp3` (8×8 grid), hill-climbs from 5 starting points,
`-fno-schedule-insns` (241w).
