# func_80084C4C — PARKED (lane exeD, 2026-09-27)

VRAM `0x80084C4C`, file `0x7544c`, size `0x264`, `asm/disc1/7544C.s`.

- Best: 1 word (27 mismatches from a 1-word shift; 1 diff line) under 2.8.1 -O2 -G0 -mno-split-addresses (era_o2_g0_cc1_281).
- Draft: `build/lanes/exeD/drafts/func_80084C4C.c` (if present).
- Levers found (21 -> 1 diff lines): a switch with the shared increment label inside case 1 (case 0 does `goto bump`, as in retail's layout); `int old` (no re-`andi` of the saved type byte); `register int r asm("$2")` for the callback result, so the tail is `lbu v1; addu v1,v1,v0; sb v1` and cc1 no longer cross-jumps it with the case `sb v0,70(s0)` stores. Last word: retail's reorg filled the `beq v1,v0(254),Lfe` slot with `li v0,255` and deleted the case-0xFE body's own `li v0,255`. Ours fills the slot from the fall-through and keeps the target copy. Flags (-fno-thread-jumps, -fno-cse-*, -fno-rerun-cse-after-loop, -fno-schedule-insns, -fno-expensive-optimizations) and case order (0xFF before 0xFE; 0xFE first) give no change or worse.
