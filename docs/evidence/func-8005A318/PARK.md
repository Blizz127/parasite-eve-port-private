# func_8005A318 — RESOLVED 2026-09-28 by lane exeC5 (see REPORT.md); history below

- VRAM `0x8005A318`, file offset `0x4AB18`, size `0xCE4` (825 words), `asm/disc1/4AB18.s`.

## This lane (exeC)

- Not re-attacked this lane (time went to the closer candidates; 10 of 29 closed). Best known state is the
  prior fan-out record below.

## Prior record

- No prior draft exists (never drafted). Large body; no C in `src/`.

## Lane exeC5 (2026-09-28): drafted from retail control flow — 1 word (0 aligned diffs)

- Draft: `build/lanes/exeC5/drafts/func_8005A318.c` (3312 bytes = 0xCE4, frame 0xA0).
  Profile: `-O2 -G8` + `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800C0E20,D_800C0E22` (scalar byte globals).
  `era_link_check`: `word mismatches=1` — only `0x8005AD6C: ROM 14500099 LNK 14500010`: the
  `bne v0,s0` of the final block's first `D_8009D048 == &D_800C0E48` test branches to the next
  test (0x8005ADB0) instead of straight to the epilogue (0x8005AFD4).
- Structure: inline copies of func_80059F08-style `sel(k)` (a `static inline` taking
  `unsigned int k` — the `(who == 0) < 2` test must not fold, so it has to be a parameter),
  func_800574A8-style lookup (p = 0 first, block-local `v`, `w`), func_8005415C `gettype()`,
  func_8005332C `lookup()` (weapon-9 loop, fresh `pp` local), func_80057D30 remove body twice
  (block-local `e`,`w`,`v`), type-descriptor select, `CAP`/`HP`/`MIN` macros with a CAP-style
  `(x > 999) ? 999 : x` clamp, `slot` reused as every later loop counter (retail keeps it in $s1),
  separate class/empty loop counters, `int unused[2]` in the last block (retail frame has one
  more dead 8-byte slot than the draft; placing it in that block keeps `tmp` at 0x10).
- The residual: jump2 `thread_jumps` must see the `lw D_8009D048` as the insn right before the
  first branch (so it pairs with the second test's `lw`); sched1 always orders `lw` before the
  `la car`, so our threading fails. Placing `car = …` earlier moves the `la` into the prior load
  delay slot (13 words); a barrier on `car` before the second test fixes the register choice
  but blocks threading the same way; nested ifs / early `return` bring back the cse canonical
  problem (5–7 words). Operand-order/alias brute force over both tests: min 1.
