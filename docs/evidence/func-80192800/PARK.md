# func_80192800 (ovl_0700, blob 0x3810, 0xC78) — PARKED (ovl14, 2026-09-28)

Scene-object draw pass (≈50 func_80190E04 / func_80191114 calls with D_801EA5E4 draw-depth
settings, camera-scroll jitter, shadow-object copies). Profile era_o2_g0.
Best draft: `build/lanes/ovl14/drafts/park_func_80192800.c` — **one misplaced instruction**
(27 diff lines; object one word short, so the tail shows as a branch-target shift).

Residual: after the four `D_801EA578[k]->f2 = v` stores, retail keeps `addu $a2,$zero,$zero`
(the next call's 3rd arg) *after* the `[3]` store and leaves the `lw [3]` load-delay slot as a
`nop`; cc1 sched1 (trace: at T-11 ready = {522 store prio 4, 538 a2 prio 1}) places the `a2`
set above the store, filling the slot.

Levers found (each verified by diff-line count):
- `Obj *D_8019C3B0[]` flat array with `[i*2]` / `[i*2+1]` — gives retail's hoisted `la` base,
  `s3` biv for the first element and `s6 = base+4` + recomputed `i<<3` for the second
  (a 2-D `[i][0]/[i][1]` or struct array reduces both to an `i*8` giv).
- `D_801EA264[2]` store duplicated into both arms of the 0x2000 test (cross-jumped back to a
  single `sb` through the hoisted base register; a store after the join goes absolute).
- `o = D_801EA578[0]; v = o->f2; if (v == 0x24) v = 0x52; else v = 0x24; o->f2 = v;`
  (separate compare constant, pointer kept in $a0, and no 1000-constant cse across it).
- `D_801EA588[0]` (separate 1-element array) + scalar `D_801EA58C` reproduce the `la $s0` +
  `lw 0($s0)` reuse for object 4 and absolute loads for object 5.
- `*(unsigned short *)&D_801EA578[3]->f2 = v` (non-in-struct store) makes the following
  `lw D_8019C9C0` depend on it, as retail.
Tried for the a2 residual: all-scalar f2 stores, moving `D_801EA5E4 = 0` above the block,
array-typed D_8019C9C0 (cse'd its address: 242 lines).
