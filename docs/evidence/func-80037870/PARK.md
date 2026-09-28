# func_80037870 — PARK (lane exeB5, 2026-09-28)

Message/dialogue glyph renderer (4 `D_800BCEA8` Pop records, TXT stream parser, DR_MODE+SPRT glyph packets).
VRAM `0x80037870`, file `0x28070`, size `0x10A0` (1064 words), switch `jtbl_80010E6C` (FB sub-op 0..9).

Best draft: `build/lanes/exeB5/drafts/func_80037870.c`.
Env: `-O2 -G8`, `MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_DISPATCH_FOLD=jtbl_80010E6C
MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009CDDC,D_8009D1F4,D_8009169D`.
Best: **linked size exact (0x10A0)**, 803 word mismatches, 143 non-aligned lines (difflib over the objdump);
the remaining differences are almost all stack-slot renumbering and one hoist decision.

## Decode (confirmed against retail)
- Record = `Pop` from src/func_800371B0.c (bitfield `Flags` at +0xC, `pos[4]`, `dig[5][6]`); fonts = 16-byte
  `Font` array at `D_80091644` (`v`@+6, `tpage`@+0xC, `clut`@+0xE); packet = `SEnt` (8-byte mode + Sprt).
- `p = (SEnt *)D_800B0E38[bank + 3]` (retail derives `D_800B0E38[bank]` as `D_800B0E44 - 12`).
- Top switch on `*s` (binary-tree compares: FA/F7/0F/F8/F9/FC/FB/FD/FF + default, `case 0x0F` falls into the
  default glyph), FB sub-switch via jump table, `do { ... } while (!stop)` glyph loop.

## Load-bearing levers found
1. **`func_80077AC4(ot, p++)`** (AddPrim with post-increment argument) — the temp becomes a DEST_REG giv `p+0`,
   so loop.c reduces all Sprt stores to retail's `s2` (and `s1` in the FA/digits inner loops) with `0x10`
   displacements, and `p` (s6) is incremented before the call. 920 → 534 diff lines.
2. `do {} while (*s++ != 0xF8);` (a `while` is folded away by cse-follow-jumps: *s is known 0xF8).
3. Separate scan pointer `t` for the `(FF|F9) FE id` marker search (reusing the cursor put it in s3).
4. case 9: `pp = xy` (retail `addiu $t0,$sp,0x10`) — also frees `$t0`, making `$t1` the reload register as retail.
   `Sprt *sq = &D_8009EC70[b].sp` gives retail's `D_8009EC78` base with `-8` for the AddPrim argument.
5. `unsigned short lh = 12; xy[1] = lh + xy[1];` (retail rematerialises 12 into `$t1`, operand order `12 + y`).
6. `mask = 0xFFF0FFFF` as a **variable set right before the record loop**, with manual word RMW
   (`*(unsigned int *)&rec->fl = (w & mask) | ...`) — reproduces retail's spilled `0x60($sp)` mask load in
   state-init and FB 6/7 (bitfield stores rematerialise lui/ori instead). 945 → 803.
7. case 6/7 inner loops as `label: if (ctl == b16) { ...; goto label; }` (non-inverted).

## Residual
- **Signed `/21` magic `0x30C30C31` hoisted by loop.c** in the FB>=10 / FC / FD blocks (spilled to an extra
  stack slot → frame 0xA8 vs 0xA0, `$s0/$s1` save slots shift, every spill slot renumbers). Retail keeps it
  block-local in `$a1`, while the unsigned `0x86186187` (FA + default glyph) *is* hoisted in both.
  Tried: no `code` temp, `do{}while(0)` macro, goto-built glyph loop (stops the hoist but kills the p+0 giv and
  the adv extension hoist), for(;;)/continue forms of the FB 6/7 loops (retail hoists `li $a1,6` before the FB-6
  loop, so that loop is loop.c-visible in retail; the for(;;) form reproduces that but also hoists `ctl+1`).
  loop.c `threshold -= 3` per move (agbcc loop.c:1930) makes this order-dependent.
- Unexplained unused retail slot `0x38` (a pseudo numbered between `bg` and `ot`).
- Retail FB-6 exit test `if (*s == 6) s++` lives inside the loop (hoisted `6`), and the `b16 + 1` store uses
  `ctl + 1` (cse2), which the current goto form does not reproduce.
