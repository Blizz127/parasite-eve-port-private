# func_8007B010 — PARKED (lane exeD, 2026-09-27)

VRAM `0x8007B010`, file `0x6b810`, size `0x280`, `asm/disc1/6B2B4.s`.

- Best: 7 words (6 diff lines) under 2.8.1 + MASPSX_FILL_CALL_DELAY_SLOT=1.
- Draft: `build/lanes/exeD/drafts/func_8007B010.c`.
- Residual / levers tried: Reproduced the archived 7-word residual. New data: with `-fno-schedule-insns` the prologue order matches (li a0,-1 after move s7,a1), but the printf-argument chain registers swap (intstr[a] chain in $v1, y in $a0). Retail has the chain in $a0 and y in $v1. `register char *y asm("$3")` fixes y (13 mismatches / 6 diff lines), but the chain goes to $a3 and a `$4` pin on `a` is ignored. A `$4`-pinned `char **pa = &intstr[a]` makes it worse (16 diff lines). 12 statement permutations of the printf block, times both schedule modes, gave best 6 diff lines.
- Measured with `tools/analysis/era_link_check.py` through `build/lanes/exeD/t.sh`; the diff-lines counts come from an aligned objdump diff.

## exeD3 round (2026-09-27)
- Still 7 words. The retail printf chain pseudo is in `$a0`. Tried: a `$4`-pinned `char **pa`, a `$3` pin on y, unprototyped and fully prototyped printf, and 4 temp orderings (145–155/160). Unchanged.

## exeD5 round (2026-09-28): still 7 words; mechanism identified
- Profile needed: 2.8.1 `-O2 -G0 -mno-split-addresses` + `MASPSX_FILL_CALL_DELAY_SLOT=1` (without `-mno-split-addresses` the result is 143).
- From the sched1 ready-list trace (`cc1 -dS`): the printf arg setup `(set $4 D_80011B28)` (insn 132) is a hard-reg
  set with REG_N_SETS($4) > 1. So it is a non-birth with priority 1, while `(set $6 …)`/`(set $7 …)` (the only sets
  of $6/$7 in the function, n_sets == 1) are births with max priority. sched1 therefore floats the `$4 = fmt` load up
  to just after the D_8009AFD5 load, $4 is busy across the sync[0] chain, and local-alloc puts the chain in $7.
  Retail has the chain in $4, so its fmt load came after `lw $7`.
- Tried: named x / `*px` / `*(intstr + a)` / fully inline arguments / `char *f = D_80011B28` (7–18). `register … asm("$4")` on a, on the
  chain pointer or on a copy is ignored by 2.8.1 (the chain stays in $7/$3), and one form miscompiles (fmt overwrites the
  pinned $4 before the arg load, 106). Parameter pin variants: only `result`-before-`mode` keeps 7, the others 13–14.
  The prologue `li $4,-1` has the same non-birth hard-reg cause.

## exeD6 round (2026-09-28): still 7 words
- Tried the asm-output lever on the printf format (`char *f; asm("" : "=r"(f) : "0"(D_80011B28), "r"(x)...)`) at every
  source position, volatile and not, with and without a separate single-set input pseudo. The best forms put the
  `intstr[a]` chain in `$4` as in retail, but the asm makes f a pseudo that sched1/regalloc treat differently. The
  `la` floats to the block top or the block order changes: 19–21 words, or 107–111 with a size shift. The prologue
  `li $4,-1` (same non-birth class) is unchanged.
- Conclusion: the residual needs sched1 to treat the hard-reg `$4 = fmt` set as a birth. There is no C spelling for
  that, and it is not a maspsx-expressible transform (register renaming).
