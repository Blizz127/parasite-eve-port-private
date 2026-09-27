> **RESOLVED 2026-09-27 (lane exeC2): EXACT — see REPORT.md.**

# func_800494AC — PARKED (lane exeC, 2026-09-27)

- VRAM `0x800494AC`, file offset `0x39CAC`, size `0xC1C` (775 words), `asm/disc1/39CAC.s`.

## This lane (exeC)

- 13 words (unchanged). Sum residual (7w): full 120-form sweep (5 association trees x 24 operand orders) -> min 13 (the draft's a+b+(c+d)); every left-chain form adds a load-delay nop (591 = shift). Retail order a,b,c,(a+b),d,+c,+d with d reusing b's reg (a0) => decided in sched1 before local-alloc. Unsized int[] declarations (no FORCE needed, same 13), volatile-cast loads, explicit temp chains, ok=0 placement: no change. Second residual (6w): li a0,N vs move s0,v0 order before the 3 func_8006322C(N, s0 = func_80062D2C(...), s0) sites — not attacked further this lane.

## Prior fan-out record (build/archive/fanout/*/results.jsonl)

- `decompA`: {"status": "parked", "best": "13 words (size exact 0xC1C), era_o2_g8 + FORCE D_800A1888,D_800A188C,D_800A1890,D_800A1894", "levers": "CLOSE_MENUS macro (5 copies); clamp as if (H+D<1000) H=H+D; else H=999 (reproduces retail copy+recompute) then ternary > 999 form for single store; unaligned Blk32 {unsigned char b[32]} for lwl/lwr struct copy (also gives the 0x40 frame, no pad local needed); per-block locals (ok flag, w, MAX w) keep temps out of s-regs; m pinned $17, s2 pinned $18; h = func_8005033C held in a variable (retail la into s3 across the call); sum spelled a+b+(c+d) keeps size; function-scope p shared by 0x1000/0x40 blocks; p reuses the s0 variable in 0x10000 path", "residual": "(1) D_800A1888..94 sum: retail loads a,b,c then add, d later; ours (a+b)+(c+d) order - 7 words; (2) li a0,6/5/0x29 vs move s0,v0 order after func_80062D2C - 6 words (isolated test gives retail order, full function doesn't)", "draft": "drafts/func_800494AC.c"}
