# func_80071A84 — PARKED (lane exeE4, 2026-09-28)

VRAM 0x80071A84, file off 0x62284, size 0x884 (545 words). Psy-Q libc `sprintf`-style
formatter: flags `- + space # 0`, `*`/digit width, `.` precision, `h l L` length loop, jump
table `jtbl_80011644` over `'L'..'x'` (cases L X c d h i l n o p s u x; default: `%%` literal or
terminate), 512-byte digit buffer written backwards, strlen/memchr/memcpy BIOS thunks
(func_80072314 / 80072324 / 80072334).

Best draft: `build/lanes/exeE4/drafts/func_80071A84.best.c`.
Env: `ERA_CC1_VER=2.8.1 MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_DISPATCH_FOLD=jtbl_80011644`,
flags `-O2 -G0 -mno-split-addresses -mhard-float`.
Result: `linked .text 2144 bytes, target 0x884, word mismatches=363` (raw count is shift-inflated:
text is 9 words short); opcode-normalized diff (registers/labels masked) = 33 lines.

## Load-bearing levers found (mismatch ladder, opcode-diff lines)
1. `fmt` is `signed char *` and its address is taken (`ap = (char *)(&fmt + 1)`) → fmt lives in
   its incoming arg slot 0x254 exactly like retail; `char` is unsigned in this cc1 (plain `char c`
   gives `lbu`+`andi`), so c is `int` read through `signed char *`.
2. `ap` must live in memory at 0x220 but NOT be volatile (retail fills a `j` delay slot with the
   `sw ap`; reorg refuses volatile). Working form: struct local
   `struct { struct Spec s; int pad; char *ap; } ctx;` — gives frame 592 and 0x220. (volatile ap:
   43; asm "m"(ap): extra address register.)
3. **`-mhard-float` is load-bearing (93 → 43).** It raises `n_non_fixed_regs` and so loop.c's
   move threshold; with soft-float the fifth halved movable (`'0'` constant, savings 6, life 85)
   is "not desirable" and `$s3='0'` is never hoisted (diagnosed from `-dL`: insn_count doubles per
   already-moved movable). With it, all four flag constants land in s7/s6/s5/s3 as in retail.
4. `spec.sign = c` for the space flag (retail `sb a1`), `w * 10 + (c - '0')` digit form (-4),
   `p = buf + sizeof buf` BEFORE `if (minus) zero = 0` (fills the beqz slot, -2).
5. Spec struct: `unsigned` bitfields minus/plus/alt/zero/hasprec/h/l/L at bits 0..7, `signed char
   sign` at +1, width +4, prec +8; template copy from `D_80094528` (12 bytes, 3 lw/3 sw).

## Residual (33 opcode-diff lines)
- R1 sched1 hoisting (≈5 sites): in the d/u/o/x va_arg blocks retail loads the flags word AFTER
  `sw ap` (same reg as the ap temp); our sched1 hoists it above; same class for `li 42` in the `.`
  block (ours fills the preceding bne slot) and the `'n'` srl/andi. `-fno-schedule-insns`
  (51), 2.8.0/2.8.0-plain (identical), -mcpu variants, -O1 ± sched: all worse or identical.
- R2 `c = *++fmt` after `*` width / `*` prec: retail `lb a1,2(a2)` (old base + 2), ours
  `lb a1,1(a3)` (new-base temp kept alive) — cse equivalence choice; `fmt++, c=*fmt`,
  `c=*(fmt+=1)` identical.
- R3 `'X'` case: retail `lui a3,%hi / j / addiu a3,%lo` (la split into a plain-`j` slot).
  No knob covers `la` before an unconditional `j` (MACRO_SPLIT is conditional-branch only) —
  knob request K-LAJ in build/lanes/exeE4/KNOB_REQUESTS.md.
- R4 `$s6`/`$s7` swapped for '-'/'+' (global-alloc order), 1–2 prologue words.

Not tried yet: haifa-style ready-list traces (-dS shows sched1 puts insn 483 flags-load before
the ap increment 479/480); a pointer-based `ap` that aliases spec (breaks the reg budget).

## Update (same lane, later): gcc-2.7.2-cdk is better than 2.8.1 here
Same draft, env `ERA_CC1_VER=2.7.2-cdk MASPSX_THREE_WORD_SYMBOL_STORE=1
MASPSX_FILL_EPILOGUE_DELAY_SLOT=1`, flags `-O2 -G0 -mhard-float` (cdk rejects
`-mno-split-addresses`): `linked .text 2144 bytes, word mismatches=380`, **opcode-diff 27**.
cdk alone fixes R2 (`lb a1,2(a2)`), the `li 42` bne-slot fill, the `p = buf+512` slot, R3 (`la`
split into the `j` slot) and R4 (s6/s7). (Neighbouring libpress leaves func_8007C214/C304/C484
also match on a 2.7.2 cc1, not 2.8.1.) Remaining:
- cdk emits the jump-table address split (`lui $2,%hi($L116)` in the beq slot / `addiu` / `sll` /
  `addu` / `lw 0($3)`); retail is ASPSX's `sll $v0,$v1,2` (slot) / `lui $at,%hi(jtbl)` /
  `addu $at,$at,$v0` / `lw $v0,%lo(jtbl)($at)`. MASPSX_DISPATCH_FOLD does not see this shape →
  knob request K-CDKJT (build/lanes/exeE4/KNOB_REQUESTS.md).
- R1 flags-load scheduling at the d/u/o/x/n va_arg sites (≈10 words incl. nops): retail keeps
  `lw flags` after `sw ap` (unfilled load delay) in d/u/o/x but hoists it in s/n; ours hoists
  everywhere. Unresolved.

## Lane exeI (2026-09-28): opcode-diff 27 → 0 on cdk; 12 register-only words left (still parked)
Best draft `build/lanes/exeI/drafts/func_80071A84.best.c`. Env: `ERA_CC1_VER=2.7.2-cdk MASPSX_THREE_WORD_SYMBOL_STORE=1
MASPSX_FILL_EPILOGUE_DELAY_SLOT=1 MASPSX_DISPATCH_FOLD=jtbl_80011644` + **new knob `MASPSX_CDK_SPLIT_DISPATCH=1`** (private
maspsx `build/lanes/exeI/maspsx`, run through `ERA_LINK_CHECK_MASPSX`), flags `-O2 -G0 -mhard-float`.
Result: `linked .text 2192 bytes … word mismatches=12`; opcode-normalized diff = **0** (every instruction and slot matches, and the
2192 is gas section padding past the 0x884 body).
1. **R1 solved: an empty `__asm__ volatile("");` right after `v = VA_ARG(ap, int);` in d/u/o/x.** In cdk (2.7.2) sched1 the volatile
   asm is a memory/register barrier, so the flags-word `lw 528` stays after `sw ap`. Retail keeps it there, with the unfilled load
   delay (27 → 8). A volatile read of the flags instead also fixes the placement, but it turns the bitfield read into `lbu` and
   defeats the later reuse of the flags register. All four sites are needed (subset sweep: 5–15).
2. **`'n'` case**: retail has `sw ap; srl v0,v1,5` with an unfilled `beqz` slot. The fix is a function-level `unsigned int fl`
   read before the va_arg, `fl = *(unsigned int *)&ctx.s; p = VA_ARG(ap, signed char *); __asm__ volatile("" : "=r"(fl) : "0"(fl));
   if ((fl >> 5) & 1) …`. The same `fl` is also used for the d-case `h` test (`fl = …; if ((fl >> 5) & 1) v = (short)v;`), which
   makes `fl` a multi-block (global) pseudo. Global-alloc then puts it in `v1`, and local-alloc cannot tie the `srl` output to it.
   A block-local `f` ties (`srl v1,v1`) or lands in `v0`/`v1` swapped.
3. **K-CDKJT** (new knob, see build/lanes/exeI/KNOB_REQUESTS.md): cdk's split-address switch dispatch is rewritten to stock 2.7.2's
   `sll`(slot) + `lw $t,$L($t)`, which DISPATCH_FOLD then turns into retail's `lui $at / addu $at,$at,$v0 / lw $v0,%lo(jtbl)($at)`.
4. **s6/s7 ('-'/'+') is a global-alloc priority truncation.** allocno priority is a double, but `allocno_compare` compares it as
   `int`. Both constants have 6 refs, so priority is 120000/L. At L = 892/894 both truncate to 134, and the tie goes to the lower
   pseudo number (`'-'`), which is wrong. exeE4's 882/884 gave 136/135, which is right. The barriers added ~10 insns of loop
   length. One more `__asm__ volatile("");` after `case 'c':` brings L to 894/896 (134/133), which fixes it (18 → 12). Each empty
   asm adds 2 to L. The lever is fragile: any later insn-count change can flip it again.
Remaining 12 words (both sites are the **address split**, register-only):
- Template copy `ctx.s = D_80094528`: retail `lui a1 / addiu a1,a1 / lw v0,v1,a0`, which is stock 2.7.2's unsplit `la $5` form,
  byte-exact. cdk splits the address: the `(high)` pseudo takes `v0` in local-alloc, and reload then gives the movstrsi
  scratches `v1,a0,a1` plus address `a2`.
- `digits = D_8001161C / D_80011630`: retail `lui a3 / j / addiu a3,a3` (ASPSX splitting an unsplit `la` into the `j` slot), ours
  `lui v0 / addiu a3,v0`.
So the retail compiler here looks like cdk's code generation *without* address splitting. cdk has no switch for that (no
`-mno-split-addresses`; `-mdebug[a-j]` no effect). Cross-check with 2.8.1 `-mno-split-addresses` on the same draft: opdiff 11. The
template and digits are right there, but R2 (`lb 2(R)` ×2), the `li 42` slot, the `la`→`j` slot (K-LAJ) and t-register movstr
scratches are wrong (109 diff lines). Stock 2.7.2 / 2.7.2.3: opdiff 137 (frame 600, extra saves).
Next steps: an assembler-level fix is not possible (the scratch registers are chosen by reload after the hi pseudo). Either find
a source form that leaves the `(high)` pseudo unallocated in cdk (reload would then load `(lo_sum (high S) S)` into one reload reg,
which is retail's shape), or return to 2.8.1 and attack R2 / the `li 42` slot.

## RESOLVED (lane exeJ, 2026-09-28): EXACT, see REPORT.md
The fix: a `$7` pin on `digits`, a `$5`-pinned opaque template pointer, three empty asms in case p, and K-CDKJT (landed).
