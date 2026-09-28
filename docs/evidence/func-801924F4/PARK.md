# func_801924F4 (room_m0205i, 0x3F8; twin m0231i func_80192548) — PARKED (lane ovl5, 2026-09-27)

Best 11 words (`build/lanes/ovl5/drafts/park_func_801924F4.c`): case-2 `s1 = (DC4(h6)/4 + 0x1000) * s1 / 4096`
product lands in `$v0` (retail `$a2`) and the `sra s1` is not delayed into the k==4 `bne` slot. Levers that
worked on the way: 4-byte colour struct as `unsigned char[4]` (block move with `addiu` base, not `lui $at`
SImode pair), sv declared before t (frame order), one `q = D_800F32D0` local for the three reads.
Tried: product/quotient temps in 3 placements, `register int m asm("$6")` pins.
