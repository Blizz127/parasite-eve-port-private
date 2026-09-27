# func_8006AD40 — LINK_EXACT

VRAM `0x8006AD40`, size `0x61C`. cc1 2.8.1, `-O2 -G0 -mno-split-addresses`.

Two opt-in maspsx passes, both load-bearing:

- `MASPSX_SINK_RETURN_ZERO_INTO_FLAG_DELAY=1` moves the epilogue `move $2,$0` into the reorder-mode `beq $2,$0,$L2` delay (as a real `addu $2,$0,$zero` inside `.set noreorder`) and puts the fall-through instruction back after the branch. Retail's entry delay is `addu $v0,$zero,$zero` (`00001021`); stock maspsx left a nop there because the branch is in reorder mode.
- `MASPSX_UNFILL_EPILOGUE_DELAY_SLOT=1` then rewrites the return to `addiu $sp,$sp,0x30` / `jr $ra` / nop.

Sink alone: 2 word mismatches (the epilogue swap). Unfill alone: 4. Both: `LINK_EXACT`, `nonzero_pad=0`, `leaf_strong_check.py` `LEAF_STRONG=EXACT`, `disc1_preflight.py --deep --only func_8006AD40` PASS.

Profile: `era_o2_g0_cc1_281_sink_return_zero_into_flag_delay_unfill_epilogue_delay_slot`.
