# func_80088980 — PARKED at 10 words (lane exeE, 2026-09-27)

VRAM `0x80088980`, file `0x79180`, size `0x4E4`. AKAO voice LFO/volume/pan/pitch update.
Best draft `build/lanes/exeE/drafts/func_80088980.c`, era 2.7.2 `-O2 -G0` (2.8.1 lacks the
retail 16-byte frame: 282).
Ladder: archived decompB draft 17 → `register int s asm("$4") = *PTR(0x1C)++; r = (U16(0x92) * s) >> 16;`
in each LFO block (sample in `$a0`, operand order `mult v0,a0`) 15/12 →
block 2 `register int x asm("$3"); register int y asm("$2") = (vol*(U16(0xA6)>>8))<<9; x = y >> 16;` **10**.
Residual 10: pan (`$v1` in retail) vs `D_8009D2C0` switch value (`$a0` in retail) swapped
(global-alloc priority). Tried: pan pinned `$3` (19), switch value pinned `$4` (60),
if-chain (127), switch value in k/r/p (10/10/60), volatile D_8009D2C0, pan types
(unsigned/short/long/uchar), pan decl order, block-scoped pan, `pan = 0x80` placement,
asm barriers on pan.

Follow-up (same lane, after func_80088344 closed with the shared-variable lever): retail's
registers imply two reused locals — `$v1` = LFO results / block-2 intermediate / pan / `k`,
`$a0` = samples / `D_8009D2C0` / pitch `p`. Writing it that way (drafts f88980.w3–w8 in the lane
dir) gives 44 (pan loses `$v1` to the sample variable on global-alloc priority), 93 when `p`
is folded into the sample variable, 271 with both pinned; inline `t` makes no difference.
The pinned draft (10) remains best.
