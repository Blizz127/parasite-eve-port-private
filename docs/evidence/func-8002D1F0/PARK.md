# func_8002D1F0 — PARKED (lane exeB, 2026-09-27)

- VRAM `0x8002D1F0`, file offset `0x1D9F0`, size `0xA68`; HP-critical palette flash
  (4 colour patterns into `D_800B00EC..` / `D_800B692C..` indexed by `D_8009CDDC`), damage
  popups, then the actor-list status walk.
- Profile, corrected this lane: **`-O2 -G8` + `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009CDDC,D_8009D250,D_8009D254,D_8009D20C`**.
  In retail only `D_8009D278` is gp-relative (`lw $x,0x508($gp)` ×7); the other four are
  absolute `lui/lw`. The prior draft used `-O2 -G0`.
- Unused local `buf[0x198]` (was `0x160`) gives the retail 448-byte frame (regs at 432..440).
- Best draft `build/lanes/exeB/drafts/func_8002D1F0.c`: 615 mismatches. Linked 2560 vs
  2664 bytes: **26 words short**, spread over the late blocks (from ROM word ~500 on).

## Residual 1 — colour-constant scheduling (sched1 priority)
Retail arm 0 loads `li 70/130/159` before the first `D_8009CDDC` load, then `li 255` and
`li 249` into the first two load-delay slots. The `-dS` dump shows cc1 already CSEs each
colour into one QI pseudo (reg 94 = 70 feeds both the `ED` and `FD` stores). gcc-2.7's
backward list scheduler then picks the highest-priority ready `li`: 70 is used first, so it
has the longest path. That `li` lands in the second-lw stall. Retail put 249 there, i.e.
its ranking was the reverse. Block-local colour variables
(`unsigned char g0 = 0x46, ...` at the top of the arm) are folded back by cse: no change.
Prior lane also tried: volatile `D_8009CDDC`, `D_8009CDDC[]`, a `Pal` struct, `switch`,
2.8.1, `-O1`.

## Residual 2 — 26 missing words in the tail (not yet analysed)
Start with `python3 build/lanes/exeB/tools/sd.py build/lanes/exeB/out/8002D1F0` after
`build/lanes/exeB/tools/d.sh 8002D1F0 A68 -O2 -G8` (with the force-absolute env).

## Second pass (exeB, 2026-09-27)
- **Tail cross-jump now matches retail.** In retail, arms 0-2 `j` into arm 3's final
  `D_800B692E` store, which is the address computation and `sb $a0` only. Each arm keeps its
  own last `lw D_8009CDDC`. cc1 instead merged two whole stores, into arm 2. The fix: write
  arm 3 in the same per-field form as the others, and read the last index in arms 1/2/3
  through distinct asm-label aliases (`extern int D_8009CDDC_b asm("D_8009CDDC")`, `_c`, `_d`),
  so no two arms' final `lw` compare equal. Draft `build/lanes/exeB/drafts/func_8002D1F0.c`:
  arm branch targets match retail, length 2656 vs 2664.
- **Remaining: `li` placement, which explains the whole length gap.** Retail loads 70/130/159
  at the top of each arm and 255/249 into the first two load-delay slots; later loads get
  `nop`s. `build/lanes/exeB/drafts/func_8002D1F0_barrier_expt.c` reproduces that shape in
  arm 0: the three constants as locals set before an empty `asm volatile("")`, then 255/249
  set, the first two stores, and a second barrier. That supports the reading that retail's
  sched1 region boundaries differed. The homes still come out permuted (70/130 in
  `$a3`/`$t0` against retail's `$t0`/`$a3`, 255/249 in `$a0`/`$a1` against `$a1`/`$a0`) under all 12
  initialisation orders. Two contrived barriers per arm is not a credible source, so this is
  not banked.
