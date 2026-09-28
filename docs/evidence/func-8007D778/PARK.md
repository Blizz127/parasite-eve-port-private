# func_8007D778 — PARKED (lane exeD, 2026-09-27)

VRAM `0x8007D778`, file `0x6df78`, size `0x280`, `asm/disc1/6DF78.s`.

- Best: 16 diff lines (48 mismatches) under 2.8.1 -O2 -G0 -mno-split-addresses.
- Draft: `build/lanes/exeD/drafts/func_8007D778.c` (if present).
- libspu _spu_t. Levers (41 -> 16): D_8009B454 non-volatile, the CHCR value as `chcr = 0x1000201; if (D_8009B44C == 1) chcr = 0x1000200;`, `$4`-pinned locals for the va_arg reads. Left: retail keeps the constant 1 in $a2 across cases (used for the `D_8009B44C = 1` stores and compares), the `lui a2,0x100` scheduled into the `j`/`jal` slots, and the case-2 srlv result register. -fno-schedule-insns is worse (33).

## exeD4 round (2026-09-27): 48 → 15 word mismatches (151/160 aligned, right size)
- Best draft: `build/lanes/exeD4/drafts/func_8007D778.best.c`. Profile unchanged: 2.8.1 `-O2 -G0 -mno-split-addresses`.
- Levers that landed:
  1. `extern unsigned int *volatile D_8009B400;` (the DMA MADR pointer). The pointer load then stays
     after the volatile `D_8009B450` store, as in retail. 150 → 152 aligned.
  2. For the case-3 `ap[-1]` read, `register int y asm("$4") = ap[-1]; D_8009B450 = y; asm("" :: "r"(y));`.
     The use-asm changes the sched1 order: the split `ori $6,$6,0x201` of `chcr = 0x1000201` becomes
     the last insn of the block and lands in the `bne` delay slot, which removes the extra
     nop word (161 → 160 words, 48 → 15 mismatches).
- Residual (15 words):
  (a) `lui $6,0x100` (the high half of chcr) is now scheduled after the barrier. Retail has it as the
      join block's first insn, where reorg copies it into both `j`/`jal` slots (0x8007D950/5C).
      Putting `chcr = 0x1000201` before the barrier moves the `ori` early again (48–51).
  (b) The case-3 size word is loaded into `$v0`; retail uses `$a0` and reloads `D_8009B450` into `$a0`
      after the `sltu`. A `$4` pin on a copy is a no-op, a `$4` pin on `size` gives 17, and a barrier gives 52.
  (c) The case-2 `srlv` result goes into `$a0` (tied to the `$4`-pinned x). Retail uses `$v0` (the shift count).
- Also tried: ternary / if-else / `chcr--` / constant-OR spellings of chcr (35–52),
  `-fno-schedule-insns` (136), volatile on the other DMA pointers (no change beyond the MADR one),
  and chcr in the two call arms (51).

## exeD5 round (2026-09-28): 15 → 12 word mismatches (154/160 aligned)
- Best draft: `build/lanes/exeD5/drafts/func_8007D778.best.c`. Profile unchanged (2.8.1 `-O2 -G0 -mno-split-addresses`).
- Lever: in case 2 the shift count and the result share one variable:
  `register unsigned int x asm("$4") = ap[0]; unsigned int t = D_8009B424; t = x >> t; D_8009B414 = t;`.
  This gives retail's `srlv $2,$4,$2` with the result in the count's register. The case-2 block is exact. A plain
  separate result is 15, unpinned x is 19, and a `$2` pin on the result is 133.
- Residual (12): only case 3. Retail's `lui $6,0x100` (chcr high) is the join block's first insn, and reorg copies it
  into the `j` slot. Ours comes after the `asm("" :: "r"(y))` barrier. Cause (from the -dS trace): `chcr = 0x1000000;` placed before
  the barrier is merged by combine into the later `|= 0x201`, split again (uids 395/396) after the barrier, and as a
  non-birth (n_sets 4) it is pinned just below the asm. Referencing chcr in the asm (to stop combine) makes CSE share the lui
  across both arms (47). The case-3 size word is also `$2` where retail has `$4`. That follows from the same y/asm construction.
