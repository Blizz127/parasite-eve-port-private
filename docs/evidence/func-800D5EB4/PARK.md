# func_800D5EB4 — PARKED (lane exeD, 2026-09-27)

VRAM `0x800D5EB4`, file `0xc66b4`, size `0x3e8`, `asm/disc1/C66B4.s`.

- Best: 3 words (5 diff lines) under 2.7.2 -O2 -G0.
- Draft: `build/lanes/exeD/drafts/func_800D5EB4.c`.
- Residual / levers tried: Retail puts `sra s0,a0,5` first in the bgez join block and `li s2,0x2400` last (bne slot). cc1 inverts them, and `li s2` is stolen into the bgez slot. Moving `s2v = 0x2400` to 5 other positions gives 3, or 76 when it goes after the if (the constant is then propagated into the call args). Splitting the /32 around the assignment gives 80.
- Measured with `tools/analysis/era_link_check.py` through `build/lanes/exeD/t.sh`; the diff-lines counts come from an aligned objdump diff.

## exeD3 round (2026-09-27)
- Still 3 words. All 16 source placements of `s2v = 0x2400` in the window around the /32 are invariant (247/250). 2.8.1 is worse (209). `-fno-schedule-insns` is worse (234). The residual is a reorg/sched1 constant tie: retail steals `li $v0,-512` into the bgez slot and puts `li $s2,0x2400` last in the join block.
