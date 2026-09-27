# func_8007C484 — LINK_EXACT

VRAM `0x8007C484`, size `0xB8`. cc1 2.7.2, `-O2 -G0`.

`MASPSX_SINK_ZERO_STORE_INTO_BEQZ=1` is load-bearing. Retail's `beqz` on `D_800C0DBC` carries `sw $0,D_800BE9EC` in the delay slot (the store runs on both paths; `*p = 0` only when the pointer is nonzero). The pass emits `.set noat` / `lui $at,%hi` / `beq` / `sw $0,%lo($at)`.

An empty `asm volatile("" : "=r"(p) : "0"(p))` after the status-1 block keeps the join `lhu`. Without it the following `bne` skips that load (`1462000e` vs `1462000d`, one word). The same source on cc1 2.8.1 is 17–21 words off.

Re-link of the recovered source: `linked .text 192 bytes, target 0xb8, word mismatches=0, nonzero_pad=0`, `LINK_EXACT`. `leaf_strong_check.py`: `LEAF_STRONG=EXACT` compiled `0xC0` (8 zero pad bytes). `disc1_preflight.py --deep --only func_8007C484`: PASS. One `jr $ra` at `0x8007C534`, delay nop.

Profile: `era_o2_g0_sink_zero_store_into_beqz`.
