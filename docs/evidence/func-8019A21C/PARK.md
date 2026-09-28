# func_8019A21C (room_m0350i, 0xFC) — PARKED (lane ovl7, 2026-09-28)

Best: 3 words, `build/lanes/ovl7/drafts/func_8019A21C.c`, `-O2 -G0` + `MASPSX_NARROW_SHIFTED_REG_LOAD=1` (K3b).
Ladder: exeD2 k3 draft 62 → `int w = p[0]` loaded once + `*(int *)p >> 21` word read + m0273i func_8019A290 if/else
shape 5 → K3b (`lw 0`+`sra 21` → `lh 2`+`sra 5`) 3.
Residual: retail `sll $v1,$v1,8` / `lui+addiu D_800966EC` / `andi $v1,0x3F00` / `addu`; cc1 schedules the `andi`
before the `la`. Tried 20 index spellings (byte-offset char* base, `(k & 0x3F) << 6`, 256-byte row struct, `* 64`,
unsigned, `+0x3F` wrap, named k/n locals, int* base local), statement-order climb over the whole block,
`-fschedule-insns`, `-fno-schedule-insns{,2}`, `-O1`, K4 `MASPSX_HOIST_BLOCK_LI{,_TAIL1}` — all ≥3.
