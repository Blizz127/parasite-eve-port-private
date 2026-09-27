# func_80080E34 — LINK_EXACT

VRAM `0x80080E34`, size `0x130`. cc1 2.8.1, `-O2 -G0 -mno-split-addresses`.

Two opt-in maspsx passes:

- `MASPSX_FILL_CALL_DELAY_SLOT=1` fills the `jal` slots (`move $a0,$0` into `func_80080CDC`, `addiu $a2,6` into `func_8007F0C8`, `addiu $a0,-1` into `func_80073A44`, `addiu $a0,%lo(func_80081110)` into `func_800824F0`).
- `MASPSX_UNALIGNED_WORD_COPY_RETURN_TEMP=1` (patch 24) renames cc1 2.8.1's movstrsi scratch `$t0` on a consecutive `lwl +3` / `lwr +0` / `swl` / `swr` quartet. The source base `$v0` (the `func_80080CDC` return) uses `$v1`. Any other base uses `$v0` (here `$s2`, the caller pointer). The load-delay `nop` is unchanged. Flag off leaves `$t0` and is 8 words off (`88430003` vs `88480003` and the seven sibling words).

`s0` is pinned to `&D_8009B6CC` so `move $s1,$a3` lands in the first `beq` delay. Both early-outs share one `return 0`.

Re-link: `linked .text 304 bytes, target 0x130, word mismatches=0, nonzero_pad=0`, `LINK_EXACT`. `leaf_strong_check.py`: `LEAF_STRONG=EXACT` compiled `0x130`. One `jr $ra` at `0x80080F5C`, delay `addiu $sp,56`. The two nop words at `0x80080E2C` stay the preceding asm span.

Profile: `era_o2_g0_cc1_281_fill_call_delay_slot_unaligned_word_copy_return_temp`.
