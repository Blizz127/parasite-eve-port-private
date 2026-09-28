# func_8005EED4 — PARKED (lane exeC, 2026-09-27)

- VRAM `0x8005EED4`, file offset `0x4F6D4`, size `0x2CC` (179 words), `asm/disc1/4F6D4.s`.

## This lane (exeC)

- 2 words (0x8005F0C4/C8 addiu a3,a3,1 vs addu v0,v0,s3 order). New this lane (all 2 unless noted): y position sweep through the 10 packet store lines (2/14/14/30/30/24/146..), y before x (4), 1+D, y+=1 (10), unsigned y, short x (2), short x+y (31), decl order, asm barrier on y (27), x operand swap (31), y+1 at the store site, u1 = wd + u0, static-inline store helper with args (3 forms, 2), -fno-schedule-insns (144) / -fno-schedule-insns2 (166) / cc1 2.8.1 (145) => both sched passes shape it; decided at a tie between equal-priority chains.
- + asm volatile("") barrier swept over 12 positions around the packet stores: 13-39 (worse). Still 2.

## Prior fan-out record (build/archive/fanout/*/results.jsonl)

- `decompA`: {"status": "parked", "profile": "era_o2_g8", "levers": "same-TU as func_8005ED18 (FT4 alloc/addPrim idiom); static inline move_pen(dx,dy) keeps retail's D_8009D128 self-store; n = code copy after the 0xFA test (s1 reorg-stolen copy); lo/wd split right after func_8005DC28; u/v/x/y locals computed before the packet stores", "note": "2 words: retail schedules 'addiu a3,a3,1' (y+1) before 'addu v0,v0,s3' (u0+wd) in the lbu load-delay window; cc1 here puts it after (int y) or before the x add (short y). ~25 order/type/pin variants tried. Draft drafts/func_8005EED4.c"}

## Lane exeC2 (2026-09-27): still 2 words

- `cc1 -dS -dR` dumps: the two words are insns 315 (`u1 = p->u0 + wd`, QI) and 322 (`y + 1`;
  combine folds the add into the `p->y1` HI store, so its LUID comes from the y-store line).
  Both have priority 2 in sched1 and sched2, so the tie falls to the INSN_LUID stable sort
  (gcc-2.x `rank_for_schedule`). Moving the y-store line ahead of the u1 line flips the tie but
  changes store order (31).
- No change (still 2): volatile read of D_8009D128 / D_8009D124, asm-label alias for
  D_8009D128, unsigned/short/uchar wd, u, y types (2-36), `(unsigned char)` / `(short)` casts,
  split `p->y1 = y; p->y0 = y;` (32), block temps for u1 or y (2), `int`/`short` y swept over 13
  positions (min 2), asm barrier on y swept over 12 positions (27, then 2), barrier on wd (7),
  barrier on the p->u0 value (77), barrier after the u1 add (2), pins y=$7 (32), wd=$19 (26),
  inner `yy asm("$7")` (2).

## Lane exeC4 (2026-09-27): still 2
- Output barrier on the `y + 1` result (`y = D_8009D128 + 1; asm("" : "=r"(y) : "0"(y));`) so combine
  cannot fold the add into the `p->y1` store (giving the add its own early LUID): 27 at the source
  line, 27 before `x`, 29 at the store line; barrier between the load and the add: 49.

## Lane exeC5 (2026-09-28): still 2
- No-code cse boundary `Lk: zz = 0; if (zz) goto Lk;` swept over every statement from the
  `func_8005DC28` call to the tail (26 positions): all 2. y0/y1 split stores around the u1 line (32),
  u3/u1 late copies (68), block temp `yy` (2).
