# func_8005382C — LINK_EXACT

VRAM `0x8005382C`, size `0x13C`. cc1 2.7.2, `-O2 -G8`.

`MASPSX_DROP_UNUSED_VAR8=1` is load-bearing. The body matches without it, and the six differing words are the frame: cc1 reserves an unused 8-byte temporary for `lim + 1 - count + count` (`vars=8`, frame 32) after deleting every use of that temporary. Retail's frame is 24, with `$s0` at 16(`$sp`) and `$ra` at 20(`$sp`). The pass rewrites a frame of exactly 32 whose only `$sp` memory operands are those two saves. The same expression on a tiny function (`return lim + 1 - count + count`) allocates the same unused slot. Without the knob: `word mismatches=6`.

Re-link with the knob: `linked .text 320 bytes, target 0x13c, word mismatches=0, nonzero_pad=0`, `LINK_EXACT`.

Profile: `era_o2_g8_drop_unused_var8`.
