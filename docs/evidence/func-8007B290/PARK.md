# func_8007B290 — PARKED (lane exeD, 2026-09-27)

VRAM `0x8007B290`, file `0x6ba90`, size `0x2c8`, `asm/disc1/6B2B4.s`.

- Best: 7 words (6 diff lines) under 2.8.1 + MASPSX_FILL_CALL_DELAY_SLOT=1.
- Draft: `build/lanes/exeD/drafts/func_8007B290.c`.
- Residual / levers tried: Twin of func_8007B010 (same printf-argument tie and prologue residual); see that entry.
- Measured with `tools/analysis/era_link_check.py` through `build/lanes/exeD/t.sh`; the diff-lines counts come from an aligned objdump diff.

## exeD5 round (2026-09-28)
- Same mechanism as func_8007B010 (see that PARK's exeD5 section): the fmt `$4` hard-reg set is a sched1 non-birth
  and the $6/$7 arg loads are births, so the chain register choice follows from that. Not closed.

## exeD6 round (2026-09-28): still 7 words
- Tried the asm-output lever on the printf format (`char *f; asm("" : "=r"(f) : "0"(D_80011B28), "r"(x)...)`) at every
  source position, volatile and not, with and without a separate single-set input pseudo. The best forms put the
  `intstr[a]` chain in `$4` as in retail, but the asm makes f a pseudo that sched1/regalloc treat differently. The
  `la` floats to the block top or the block order changes: 19–21 words, or 107–111 with a size shift. The prologue
  `li $4,-1` (same non-birth class) is unchanged.
- Conclusion: the residual needs sched1 to treat the hard-reg `$4 = fmt` set as a birth. There is no C spelling for
  that, and it is not a maspsx-expressible transform (register renaming).
