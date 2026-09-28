# func_8007B558 — PARKED at 83 words (lane exeE, 2026-09-27)

VRAM `0x8007B558`, file `0x6BD58`, size `0x40C`. libcd CD_cw (command writer + sync wait).
Best draft `build/lanes/exeE/drafts/func_8007B558.c`, 2.8.1 `-O2 -G0 -mno-split-addresses`
+ `MASPSX_FILL_CALL_DELAY_SLOT=1` + `MASPSX_SYMBOL_LOAD_DEST_TEMP=1`.
Ladder: archived tooling2 draft 105 → result copy as `static inline void copy8(d, s, n)`
(same helper that closed func_8007AAB4) 88 → `register unsigned char *param asm("$16") = param0;`
83 (com then lands in `$s1` like retail).
Residual: prologue pair order (param pair saved after com's), `func_8007B010(0, 0)` gets
`move a1,a0` (retail `move a1,zero`, 2.8.1 zero-arg CSE), the sync/table block (retail keeps
`&D_8009B294` in `$a1`, `com<<2` in `$a0`, and loops the param FIFO off `la D_8009B0FC` + 256
as a pointer), wait-loop register numbering. Pointer-local rewrites of the sync/table block
regress (183–191); pinning com `$17` as well regresses (188).

## Lane exeE3 update (2026-09-28): 83 → 5 words
Draft `build/lanes/exeE3/drafts/func_8007B558.best.c`; env `ERA_CC1_VER=2.8.1 -O2 -G0
-mno-split-addresses` + `MASPSX_FILL_CALL_DELAY_SLOT=1 MASPSX_SYMBOL_LOAD_DEST_TEMP=1
MASPSX_FILL_BRANCH_MACRO_SPLIT=1` (the last is new and load-bearing: splits the `la
D_8009B0FC` across the `beqz` delay slot, 182 → 135).
Levers (in order): table base as a local `st = D_8009B0FC[0]` for the `st[com]` status test and
a loop bound pointer `np = st + 64` read as `np[com]` (retail's `la v1,sym; addiu 256; addu`
chain, 83 → ~135 raw / sync block aligned); `D_800A347C++ > 0x3C0000` (retail's `move v1,v0`
copy); declare the sync pointer `s` *inside* the `if (D_8009B294[0] == 0)` block (retail tests
through a temp `a0` and copies to `s2` later) (→ 24); printf args `int a0 = s[0]; char *i1 =
inames[s[1]];` (→ 13); drop the `$16` param pin and give `param` one empty `asm("" : : "r"(param))`
use inside the param-FIFO loop (loop-weighted priority; fixes the prologue order and s0/s1) (→ 5).
Residual (5 words):
1. `jal func_8007B010` slot `move a1,a0` vs retail `move a1,zero` — 2.8.1 post-reload CSE;
   needs knob K-RCSE (spec in build/lanes/exeE3/KNOB_REQUESTS.md; asked lane exeD6).
2. 4 words: `s[0]` chain for `inames[s[0]]` lands in `a3` (retail `a0`): sched1 (backward list
   scheduling) issues the priority-1 `a0 = &D_80011B28` early, so `a0` is live across the chain
   and local-alloc can't use it. Tried: arg locals in 9 orders, `unsigned char`, pin `$4`
   (+barrier: 99), named fmt/name locals — none moves the fmt load.

## Lane exeE4 update (2026-09-28): 5 → 4 words
Draft `build/lanes/exeE4/drafts/func_8007B558.best.c` (same env as exeE3).
1. **reload_cse word closed in C**: `{ int z = 0; __asm__("" : "=r"(z) : "0"(z)); func_8007B010(z, 0); }`
   — the asm-output zero for ARG 1 keeps `$4` from being "known 0" when `$5 = 0` is emitted, so
   2.8.1 reload_cse no longer rewrites it to `move a1,a0`; retail `move a0,zero` (beq slot) and
   `move a1,zero` (jal slot) both reproduced. (Arg-2 asm zero: 215; `(short)0`: 5.) The landed
   `MASPSX_RELOAD_CSE_CONST=1` cannot close it: `$4`'s zero crosses label `$L53`, where the knob
   resets, and the knob also rewrites three sites retail keeps (`addu v0,s0,a0` ×2,
   `move v0,a0` ×2) → 9 words. Do not use it for this leaf.
2. **Residual 4 words, cause proven** (`-dl`/`-dS` dumps + gcc sched.c `adjust_priority`): in the
   timeout printf block the chain `lbu/sll/addu/lw` for `inames[s[0]]` lands in `a3` instead of
   `a0` because sched1 places `(set (reg:SI 4) (symbol_ref D_80011B28))` (insn 354) early: it is a
   hard-reg set, not a *birthing* insn, so it never gets the max-priority boost that the chain's
   pseudo sets get, and its latency-1 priority loses to the loads. `$4` is then live across the
   chain in `.lreg`, and local-alloc ties the chain to `$7`. Retail's placement (right before the
   jal) implies the fmt was a birthing pseudo or a 2-insn split `la` (priority 2).
   Tried: `char *f = D_80011B28` (combine folds it back into `$4 = sym`, 4); `register char *f
   asm("$4")` (4); asm-output `f` (loop.c hoists the asm'd invariant out of the wait loop, 244);
   volatile-asm barrier after precomputed args (chain fixed but `sw 16(sp)`/`$5..$7` held after the
   barrier, 16–17); `$4`-clobbering asm (11–13); pin `$4` on the index / address (4 / 105);
   whole-function `-msplit-addresses` (253). A per-symbol split-`la` would need a knob-level
   answer; not requested (would still need cc1-level sched effect, so maspsx can't do it).

## Lane exeF (2026-09-28): still 4 words (best stays exeE4's draft)
Mechanism refined from the 2.8.1 `-dS` dump: `$4 = &D_80011B28` (insn 355) has only an anti-dep
on the preceding call (priority 1), while every chain insn has priority 2 (volatile `lbu`, cost 2),
so backward sched1 always emits the fmt set before the chain. `$5 = D_800A3480` survives as a
pseudo load + copy (combine leaves memory args alone), which is why *it* is not a problem.
Tried (all worse or equal): hard-reg pins of the index / address / whole chain (`register ...
asm("$4")`) — gcc's arg setup clobbers the pinned `$4` (`la $4,fmt` before `lw $7,0($4)`,
**wrong code**; pin forms that looked like 4/10 words are the pin being ignored or rewritten);
asm-output fmt pseudo with a dependency on the chain value (`"0"(D_80011B28), "r"(n0)`: reload
materialises the `la` and sched2 hoists it again, 103–109); `$4`-clobbering asm consuming the
chain value (10, chain lands in `v1`); volatile barrier after precomputed `q`/`q2`/`i1` (13: post-
barrier order `lw a1; sw; lw a2; lw a3; la a0` is retail's, but the s[0] chain still lands in
`a3`, and the barrier moves the whole chain ahead; draft build/lanes/exeF/drafts/func_8007B558_barrier.c).
Next idea not tried: raise the fmt set's priority through an output/anti dependence on a
priority-≥2 insn that touches `$4` without code.
