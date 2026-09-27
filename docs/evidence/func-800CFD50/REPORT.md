# func_800CFD50 — LINK_EXACT

VRAM `0x800CFD50`, size `0x144`. cc1 2.7.2, `-O2 -G0`.

`MASPSX_LOAD_BEFORE_STACK_HALF=1` is load-bearing. The two angle steps match, including the `bgez` delay `addu` of the difference and the unfilled `jr` delay. The first step's multiply schedules a volatile `sh $v1,0($sp)` before `lhu $v0,0($a1)`. Retail does the `lhu` first. The pass swaps that adjacent pair (the `#.set volatile` comments stay between them and emit no code). The second step is already `lhu` then `sh`, so it is left alone.

Without the knob: `word mismatches=2` at `0x800CFDDC` (`94a20000` vs `a7a30000`) and `0x800CFDE0`. With it: `linked .text 336 bytes, target 0x144, word mismatches=0, nonzero_pad=0`, `LINK_EXACT`. `leaf_strong_check.py`: `LEAF_STRONG=EXACT` compiled `0x150`.

Profile: `era_o2_g0_load_before_stack_half`.
