# func_800562A4 — PARKED (lane exeC, 2026-09-27)

- VRAM `0x800562A4`, file offset `0x46AA4`, size `0x880` (544 words), `asm/disc1/46AA4.s`.

## This lane (exeC)

- Not re-attacked this lane (time went to the closer candidates; 10 of 29 closed). Best known state is the
  prior fan-out record below.

## Prior fan-out record (build/archive/fanout/*/results.jsonl)

- `decompA`: {"status": "parked", "profile": "-O2 -G8 + FORCE_ABSOLUTE D_800A1F84,D_800C0E48,D_8009D05C", "levers": ["sel()/get() inlines from func_80052E30/func_80055760", "GROUP ternary macro (distributes like retail)", "count_bits/clear_bits inlines", "get variants: early-return inline, single-exit, caller-side range macro; p pinned $5"], "note": "first draft ~500 words (mostly shift): retail keeps the out-of-range p = 0 separate from the in-range none-found res = 0 (two copy levels v0 -> a1), keeps the running count n in s5 and hoists li 1 into s3 for the bit shift; none of the get() spellings reproduce both within the time box. Draft drafts/func_800562A4.c"}

## Lane exeC3 (2026-09-27) — 536 raw → 22 raw words (size exact, 0x880)

Best draft `build/lanes/exeC3/drafts/func_800562A4.c` (era `-O2 -G8`,
`MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800A1F84,D_800C0E48,D_8009D05C`). Levers that landed (ladder
536 → 462 → 418 → 354 → 348 → 340 → 39 → 31 → 27 → 22):
1. `one = 1;` assigned at the TOP of each first-style loop body (unconditional) → loop.c hoists it
   into the preheader as retail's `li s3,1` after the `blez` guard (a function-level init lands
   before the guard; a literal `1 <<` is never hoisted).
2. First-style loop body: `int *w = (int *)((j >> 5) * 4 + D_8009D058); int old = *w; int sh = j & 0x1F;`
   then `if (g == GROUP(q)) v = old | (one << sh); else v = old; *w = v;` (one store at the join;
   integer-add address keeps `addu a3,v0,v1`; `g == GROUP` operand order).
3. The `(D_8009D04C ? 2 : 1) != 2` test goes through an inline `npass2()` (retail keeps the
   `li v1,1 / li v1,2 / li v0,2 / bne` sequence; the direct expression folds away).
4. Else-style loop: `int *w2 = w; int tt = q[6];` and
   `if ((unsigned int)(tt - 0x13) < 3 && g == GROUPC((unsigned char)tt, q)) bit = 1; *w = *w2 | (bit << (j & 0x1F));`
   where `GROUPC(c,q)` tests the narrowed value `c` for the `!= 0 / < 8 / -4` arm and re-reads `q[6]`
   for the `< 0x13 / - 0x12` arm → retail's `move t0,a2` copy and `andi a0,a0,0xff`.
5. count_bits: `int *b = (int *)D_8009D058; one = 1;` inside the loop (preheader `lw a3` then `li t0,1`).
6. The top `get(idx)` result goes into the loop variable `q` (same pseudo as the loops) → `a1`.

Residual (22 words, pure register permutation): (a) first-style loops keep `w` in a2 / `sh` in a3
(retail a3/a2), both passes; (b) callee-saved homes after pass 1: ours was=s5, n=s2, pass-2 one=s3;
retail was=s3, n=s5, pass-2 one=s2. Tried: declaration-order permutation search (no effect), pins
(p $5 breaks the p=0 path merge; n $21 / was $19 → 45), separate one1/one2 pseudos (t/one1 swap, 541),
w2/sh copies and asm uses in the first loop (no effect).
