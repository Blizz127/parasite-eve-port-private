# func_80080F98 — LINK_EXACT

VRAM `0x80080F98`, size `0x178`. cc1 2.8.1, `-O2 -G0 -mno-split-addresses`.

Three opt-in maspsx passes, each load-bearing:

- `MASPSX_SWAP_S2_S1_SAVE_PAIRS=1` swaps the prologue quartet `sw/move $s1,$a0` then `sw/move $s2,$a1` into retail's `$s2` pair first. Without it, 4 words.
- `MASPSX_LOAD_DELAY_NOP_BEFORE_LABEL=1` together with `MASPSX_NOP_BEFORE_LABEL_SKIP_EMPTY_APP=1` moves the load-delay nop to before `$L5`. An empty `asm` barrier emits `#APP`/`#NO_APP` between that label and `.set noreorder`, so patch 14 alone leaves the nop after the label and the `j` is one word early (`08020438` vs `08020437`).

Swap alone: 1 word. The nop pair without the swap: 4 words. Swap plus patch 14 without the empty-`#APP` skip: 1 word. All three: `LINK_EXACT`, `nonzero_pad=0`. `-fno-thread-jumps` is not required.

Profile: `era_o2_g0_cc1_281_load_delay_nop_before_label_nop_before_label_skip_empty_app_swap_s2_s1_save_pairs`.
