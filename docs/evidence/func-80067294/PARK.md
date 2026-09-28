> **RESOLVED 2026-09-28 (lane exeH): EXACT — see REPORT.md.**

# func_80067294 — PARKED (lane exeC, 2026-09-27)

- VRAM `0x80067294`, file offset `0x57A94`, size `0x3E4` (249 words), `asm/disc1/57A94.s`.

## This lane (exeC)

- Not re-attacked this lane (time went to the closer candidates; 10 of 29 closed). Best known state is the
  prior fan-out record below.

## Prior fan-out record (build/archive/fanout/*/results.jsonl)

- `decompB`: {"status": "parked", "best": 212, "profile": "era_o2_g0_expand_div", "note": "Render_DrawSpriteEntry: structure/cc1 2.7.2 + EXPAND_DIV correct (mod guards match); P_TAG addPrim; short x/y with int sum copies (sx/sy) reproduce retail move a2,a1 / t8,a3. Residual: retail entry block is load-in-order (e->cells lw not hoisted above cam loads; nop load slots unfilled) while our sched1 hoists e->cells and cam->z; volatile Cam*, Cam*volatile, reordering c, -O1, no-sched all worse; 2.8.1 same hoist. Mismatch count shift-inflated (~20 real)."}

## Lane exeC4 (2026-09-27) — 212 → 108 aligned (237 raw, shift-inflated); structure now matches

Draft `build/lanes/exeC4/drafts/func_80067294.c` (`-O2 -G0`, `MASPSX_EXPAND_DIV=1`).
1. **The decompB `SPRT16` was 24 bytes** (`int sx, sy` members); retail strides the sprite array by 16
   (`sll v0,t7,4`, `addiu t0,t0,16`). Fixed to `{tag; code; short x, y; unsigned uv;}`.
2. **Index form** `p16[i]`, `&p8[i * 2]`, `c[i]` in both loops (lever from func_80066F60): loop.c's
   reduced givs now keep `8(t0)`/`10(t0)` displacements (the pointer walkers gave a `p16+10` giv).
Residual: register allocation only (instruction sequence aligned): ours needs s2 (frame 32 vs 24):
`c` base → s0 (retail t9), z → s1 (s0), ot → s2 (s1); e → t4 (t3), n → t8 (t7), x → a3 (a2),
y → t9 (t8), and the loop walkers don't inherit the base registers (retail `move t5,t0; move t0,t2;
move t2,t9`). `cc1 -dl` priorities: e 0.43, ot 0.05, p16 0.16, p8 0.18, c 0.08, n 0.14, x/y 0.08,
z 0.06. Asm-use boosts on `c` (1–3 operands) did not help (110–116 aligned). Next: the 66F60
recipe — asm-use priority boosts per pseudo, `Spr *s = &p16[i]` giv-order form, checking which
pseudo takes t0..t2 first.

## Lane exeC5 (2026-09-28): 108 → 92 aligned

- Greedy sweep of `asm("" : : "r"(V), "r"(V))` priority boosts (vars e/ot/p16/p8/n/x/y/z/c/sx/sy/i
  × every statement boundary; `build/lanes/exeC5/greedy67.py`). Round 0 best: boost `p16` right
  after the `p8 += n * 2` block → 92 aligned (draft `build/lanes/exeC5/drafts/func_80067294_greedy.c`).
  The sweep was killed by the host for low memory before round 1; resume from that draft.
- Allocation facts: ours is retail shifted one register up in the t-bank (e t4/t3, p16 t7/t2,
  p8 t1/t0, n t8/t7, i t5/t4, x a3/a2, y t9/t8, z s1/s0, ot s2/s1); `c` lands in s0 because
  t8/t9 are taken — retail `c` = t9. The p16 loop walker is allocated first (priority 0.79) and
  takes a2, which retail gives to `x`.

## Lane exeF (2026-09-28): 92 → 79 aligned
- Resumed exeC5's greedy `asm("" : : "r"(V), "r"(V))` sweep sequentially (build/lanes/exeF/greedy67.py,
  log greedy67.log): round 0 on top of exeC5's draft found `e` boosted at line 81 → 79 aligned; round 1
  found no further improvement. Draft build/lanes/exeF/drafts/g67_greedy.c.
- `-fno-strength-reduce`: 176 (retail's loop walkers are reduced givs). Still register allocation.
