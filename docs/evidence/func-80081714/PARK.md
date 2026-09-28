# func_80081714 — PARKED at 4 words (lane exeD4, 2026-09-27)

VRAM `0x80081714`, file `0x71F14`, size `0x2C4` (`asm/disc1/71F14.s`). CD path-table loader, the
sibling of func_80081A7C. It reads the PVD (sector 16), checks "CD001", reads the path-table
sector, and fills the 128-entry directory table from entry 1 (num/parent/pos/name).

- Best draft: `build/lanes/exeD4/drafts/func_80081714.best.c`. Profile: 2.7.2 `-O2 -G0` +
  `MASPSX_SYMBOL_LOAD_DEST_TEMP=1 MASPSX_FILL_CALL_DELAY_SLOT=1 MASPSX_FILL_EPILOGUE_DELAY_SLOT=1`.
  Result: 176/177 aligned, correct size (0x2D0 linked, including pad), word mismatches=4. The archived tooling2 draft was 118/177.
- Levers that landed:
  1. Index the table as `extern DsDir D_800A3CB0[]` (splat already names entry 1's
     num/parent/pos as D_800A3CB0/B4/B8) with `D_800A3CB0[i].x`. Retail's i-based symbol+offset
     stores (`sw $s3,%lo(D_800A3CB0)($at)` with off = i*44) and the hoisted `&[1].pos` / `&[1].name`
     bases follow from this. `(D_800A3C84 + 1)[i]` makes a strength-reduced entry pointer (116–124),
     and `D_800A3C84[i + 1]` makes n-based addressing (118). 124 → 164.
  2. A `do { … } while (i < 128 && p < end)` inside `if (p < buf + 2048) { end = buf + 2048; … }`.
     This gives retail's `addiu $v1,$s1,2048` guard temp plus the separate `end` register copy
     `addu $s5,$v1,$zero`. 164 → 171.
  3. `{ int par = p[6]; n = i + 1; num = n; parent = par; }`: the `lbu 6(p)` comes before `n = i + 1`. 16 → 9.
  4. The first `func_80081D18(1, 16, D_800A52B0)` uses the literal. `buf = D_800A52B0` and the
     `asm("" : "=r"(buf) : "0"(buf))` barrier come after that call (before the memcpy), so `buf + 1` and
     `buf + 140` stay register-based, and sched2 puts `li $a0,1`/`li $a1,16` first as in retail. 9 → 4.
- Residual (4 words): in the loop preheader, the `end` copy `addu $s5,$v1,$zero` is scheduled
  before the hoisted `lui/addiu $s4,&D_800A3CB8` and `addiu $s6,$s4,4`. Retail has it after them,
  which is a sched2 LUID tie: the copy's insn precedes the loop.c movables. Tried: `end` assigned at the
  loop top (still moved first), at the loop bottom or in the middle (not hoisted; buf stays live, 96–171),
  `p < buf + 2048` in the loop test (buf live, 96), and `-fno-rerun-cse-after-loop` / `-fno-schedule-insns{,2}`
  (11–109). 2.8.1 is worse (55).
