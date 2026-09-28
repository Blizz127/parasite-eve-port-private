# func_8008486C — PARKED (lane exeD, 2026-09-27)

VRAM `0x8008486C`, file `0x7506c`, size `0x27c`, `asm/disc1/7506C.s`.

- Best: 7 words (2.7.2+p8+p3); 8 diff lines on 2.8.1+p8 under 2.8.1 -O2 -G0 -mno-split-addresses + MASPSX_FILL_CALL_DELAY_SLOT=1.
- Draft: `build/lanes/exeD/drafts/func_8008486C.c`.
- Residual / levers tried: On 2.8.1 the archived `a0[0x34]` reload residual matches. What remains is (a) `addu t2,t0,zero` (a 2.8.1 CSE of the zero init), (b) cc1 stealing `li v0,3` into the `bne v1,7` slot after it skips the now-redundant `lbu v1,232(s0)`, and (c) retail filling two branch slots with `addiu v0,zero,1`, which sets the branch-tested `$v0` (see KNOB_REQUESTS.md K1). cc1 reorg never does (c). Flags -fno-thread-jumps / -fno-cse-follow-jumps / -fno-cse-skip-blocks / -fno-rerun-cse-after-loop give no change.
- Measured with `tools/analysis/era_link_check.py` through `build/lanes/exeD/t.sh`; the diff-lines counts come from an aligned objdump diff.


## exeD round 2 (2026-09-27)

- Best: 7 diff lines (2.8.1 + MASPSX_FILL_CALL_DELAY_SLOT) under 2.8.1.
- Draft: `build/lanes/exeD/drafts/func_8008486C.c` (if present).
- New lever: `*(volatile unsigned char *)(a0 + 0xE8) == 3` stops reorg skipping the now-redundant `lbu` and stealing `li v0,3` (8 -> 7). Left: (a) the outer-loop giv init `move $10,$8` (loop.c derives the i*5 offset giv from the biv register, where retail has `move $10,$0`). Unchanged by -fno-rerun-cse-after-loop, a `for` form, `i = 0` inside the guard, an explicit k += 5 offset var (9), or a barrier on i (22); cc1 2.8.0 is identical and 2.91.66 is worse. (b) Two branch slots where retail puts `addiu v0,zero,1` (setting the tested $v0), and the tail order v0/v1/a0 is fully reversed from ours (a0/v1/v0). So knob K1 alone would not close it and was NOT implemented.

## exeD3 round (2026-09-27)
- Residual (b), the `li v0,1` into the branch slot, is exactly the patch-6 shape (MASPSX_FILL_BRANCH_DELAY_SLOT). It does not fire because of patch 6's compare-only fidelity gate, and the slot target is a void epilogue. A widened gate (li to the tested reg, target a void return) would fix one site. The loop giv and the reversed tail order remain.

## exeD5 round (2026-09-28): root cause of residual (a)
- `addu $10,$8,$0` (the giv init as a copy of i) is NOT from loop.c. `-dL` shows loop.c's giv init as
  `(set (reg 165) (const_int 0))` ("Biv 82 initialized at insn 46: initial value 0"), and it is still `const_int 0`
  through .cse2/.combine/.lreg. The copy appears after reload: 2.8.1's post-reload `reload_cse_regs` sees
  `$8 = 0` (the i=0 in the guard's delay slot) and rewrites the `li $10,0` as a register copy. reload_cse resets only
  at CODE_LABELs, so the guard jump between i=0 and the preheader does not stop it. A C fix needs the i=0 set to be
  invisible to reload_cse at the preheader (e.g. a real label between them), or i's zero to come from somewhere reload_cse cannot
  track. Residual (b) (two `li $2,1` branch-slot fills) is a widened patch-6 (MASPSX_FILL_BRANCH_DELAY_SLOT) knob and
  still needs the preheader order `li $2,1` first. Current (2.8.1 + FILL_CALL): 156/159 aligned.
