# func_80197230 (room_m0273i, 0x130) — PARKED (lane ovl5, 2026-09-27)

Best: 15 words (`build/lanes/ovl5/drafts/park_func_80197230.c`). Same residual as ovl3's m0350i func_801937B4
park and m0273i func_80194470 (21w, `park_func_80194470.c`): retail materialises the `k == 4` compare constant
(`li $v1,4`) and loads `D_800E1204[k]` *after* the `D_800966EC` table read, with k in `$a0` and v directly in
`$a1` (the func_80077AA4 arg). cc1 schedules the D_800E1204 load early and allocates k/v to `$5`/`$6` (+1 move).
Tried: every order of t/k/c/v (and an `i = t` copy local, all legal orders), unsigned t/k, `unsigned short v`,
direct `D_800F336C` compare, ternary / split `+4 then +5` forms, `(short)(c+0x800)` at the call, register pins
`asm("$4")`/`asm("$5")` on k/v (ignored or worse). Likely affects all ~0x130-byte m0273i/m0350i sprite bodies
(func_8019A398, 8019A068, 80196F2C, 80199950, 80193870, 80198060, m0350i 80197594/80195564/801968D8...).

## Lane ovl7 retry (2026-09-28) — 15 → 10 words

Best draft `build/lanes/ovl7/drafts/func_80197230.c` (flags `-O2 -G0 -fno-schedule-insns`, no maspsx knob).
Mismatch ladder (all `build/lanes/ovl7/t.sh room_m0273i func_80197230 <draft> <flags>`):

| form | flags | words |
|---|---|---|
| ovl5 draft | -O2 -G0 | 15 |
| + `register int four asm("$3") = 4` at block top, `k == four` | -O2 -G0 -fno-schedule-insns | 17 (pre-block now retail order; only s-reg perm + post-call) |
| + `register void *a1 asm("$17") = a1_` | same | 14 (t/c swapped in $16/$18) |
| + `asm("" : : "r"(t))` before the call (raises t's global-alloc priority) | same | 11 |
| + `us = func_80077AA4(0, v)` then `{ register void *p asm("$4") = a1; func_800CEE20(p, ...); }` | same | **10** |

Residual (10w): only the post-call `(short)c` sign-extend (`sll/sra $6,$18,16`) — retail schedules it after
`move $a1,$0`; without sched1 cc1 precomputes the complex register arg first. Tried: pinned `$5`/`$6` locals,
short prototype params, a separate `w` local for the table arg (19), `sc` local — all 10+.

Mechanism (from `-dS/-dl/-dR` RTL dumps, `build/lanes/ovl7/w/dump/`): the retail `li $3,4` is only possible if
local-alloc sees the `k == 4` constant pseudo live while `$2` holds the table value — i.e. retail's sched1 put the
li right after the `D_800966EC` load (load-delay fill) and did NOT hoist the `D_800F336C` load. Our sched1 hoists the
k load to just after the first `sll`, so the constant gets `$2` and sched2 cannot move it up. With sched1 off the
pre-block is exactly right, but the post-call arg block then lacks sched1's reordering (a0 move first, sign-extend late).
So retail looks like sched1 *on* with a different pre-block priority picture. Also tried: volatile on every
subset (≤3) of the five globals (best 15), `asm volatile("")` barriers (51), ERA_CC1_VER 2.6.3/2.7.2.3/2.7.2-cdk/2.8.0/2.8.1 (15–59), K4.
Sibling func_80194470 (21w): the same recipe gives 29/49 — there the retail li 4 lands in `$3` after t's last use.
