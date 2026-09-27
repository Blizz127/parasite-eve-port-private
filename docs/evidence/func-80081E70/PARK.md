# func_80081E70 — PARKED (lane exeD, 2026-09-27)

VRAM `0x80081E70`, file `0x72670`, size `0x2dc`, `asm/disc1/72670.s`.

- Best: 1 word (LINK mismatches=1) under 2.8.1 -O2 -G0 -mno-split-addresses (era_o2_g0_cc1_281).
- Draft: `build/lanes/exeD/drafts/func_80081E70.c`.
- Residual / levers tried: Closed the second archived residual: `func_8007EE84(9, y, 0, -1)` where `y` comes from `asm("" : "=r"(y) : "0"(0))`, so 2.8.1 CSE cannot reuse `$a1` for the `$a2` zero. That leaves `move a2,zero`. Remaining word: the first `r->cb(1, result, buf)` gets `move a0,$s3` (the `$19`-pinned `one`) where retail has `li a0,1`. A barrier on `one` inside the first branch fixes that word (e11/e12). But in every placement tried (20 variants: volatile and non-volatile, before/after five statements), sched1 then flips `li s3,1` ahead of `andi s0,s0,0xff` in block 2. The tie-break is sched1 register weight; dump `-dS` shows insn 41/39 order swapped. `-fno-schedule-insns` gives 8-9 lines. Other flags tried: -fno-rerun-cse-after-loop, -fno-cse-follow-jumps, -fno-cse-skip-blocks, -fno-expensive-optimizations, -fno-strength-reduce.
- Measured with `tools/analysis/era_link_check.py` through `build/lanes/exeD/t.sh`; the diff-lines counts come from an aligned objdump diff.
