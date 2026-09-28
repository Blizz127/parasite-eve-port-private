# func_80041108 — PARK (lane exeB5, 2026-09-28) — near miss

Memory-card operation state machine (switch on `D_800A0ED4[idx].f1`, `jtbl_80010F6C`, 16 entries).
VRAM `0x80041108`, file `0x31908`, size `0xF18` (966 words). Next row `0x32820` = func_80042020 (no resume row).

Best draft: `build/lanes/exeB5/drafts/func_80041108.c`.
Profile (new): `-O2 -G8`, `MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_DISPATCH_FOLD=jtbl_80010F6C
MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009EE70,D_80092224,D_80092230,D_800A1704,D_800A1838,D_800A1854,D_800A1864,D_800A186C`.
Best: linked size exact, **39 word mismatches**, 32 non-aligned lines, all in three spots (below).

## Load-bearing levers (ladder, unaligned lines)
- Decode from asm (pc_port decode is partial): 493.
- `-G8` + FORCE_ABSOLUTE with `extern char D_8009EE70;` (scalar): retail rematerialises `&D_8009EE70` for every
  call (cheap gp-rel constant at cse time, forced absolute by maspsx) — same trick as func_80040F80. 493 → 269.
- Retry tail macro uses `switch (c->f7) { case 1: ...; case 2: ...; }` (if/else-if cross-jumps whole tails). → 170.
- Case 2 init loop counter `n` (not `i`). → 151.
- Case 11 keeps the name in a variable (`nm = &D_8009EE70;` after the first sprintf) and a separate `up` for
  `D_800A0ED4 < c` (retail `$s1`/`$s2`). → 66.
- Case 10 entry pointer in two steps: `e = (CardEntry *)((char *)c + f3 * sizeof(CardEntry)); e = (CardEntry *)((char *)e + 0x1C);`
  (retail `addu a2,v0,s0; addiu a2,a2,28`). → 32.
- Case 2 slot search through `CardRec *r = &D_800A0ED4[idx]` (retail recomputes the base and walks it).
- Case 3 search as `if (f6 < f2*2) { cur = f6; while (1) { ...; c->f6 = ++cur; if (cur >= f2*2) break; } }`.
- Case 3 `if (c->f3 != 0xFF) { open...; RETRY } c->f1 = 12; D_800A1838 = 0;` (tail shared with case 2).

## Residual (3 spots)
1. Case 2 clear loop: retail counter in `$a0`, pointer giv in `$a1`, constant 2 first; ours swaps a0/a1
   (tried counter var i/n/k/fd/r, do/while/for forms, early `n = 14`: all identical).
2. Case 2 dir loop: sched1 hoists `a0 = &dir` above the `c->count += dir.size >> 13` update (retail keeps it
   next to `jal nextfile`). RTL order before sched is count-then-arg (checked with `cc1 -dS`); tried pointer var,
   split statements, `/ 8192`, `(unsigned)`.
3. Case 3 loop: retail hoists `lim = f2*2` into `$a1` (copy of the entry test value) and keeps `cur` in the entry
   lbu register (`addiu v0,a0,1; sb v0; move a0,v0`); ours re-reads f2 each iteration (load after the mid-loop
   break is not hoisted) — the while-test-first forms hoist lim but add a cur copy.
