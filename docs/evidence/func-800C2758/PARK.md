# func_800C2758 — PARKED at 96 words (lane exeE, 2026-09-27)

VRAM `0x800C2758`, file `0xB2F58`, size `0x398`. Script-VM step. Archived decompB draft (96).
`int done` (instead of `unsigned char`) swaps ret/done back to retail's `$s6`/`$s7` (76) but
retail's `andi v0,s7,0xff` loop test proves `done` is `unsigned char`, so that draft
(`build/lanes/exeE/drafts/func_800C2758.c`) is a diagnostic, not a candidate. Other typing
probes: `int/unsigned hi` (229), `int lo` (191), `int h` (102), init/decl order (96–100).
Residual is callee-saved numbering (op/h/zext-hi/lo) plus local temp choice in the
`reg[] += / -= lo` arms (retail index in `$a0`, base in `$v0`).
