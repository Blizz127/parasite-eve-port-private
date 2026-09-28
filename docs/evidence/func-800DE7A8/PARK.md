# func_800DE7A8 — PARKED (lane exeD, 2026-09-27)

VRAM `0x800DE7A8`, file `0xcefa8`, size `0x288`, `asm/disc1/CE8A8.s`.

- Best: 16 words under 2.7.2 -O2 -G0.
- Draft: `build/lanes/exeD/drafts/func_800DE7A8.c`.
- Residual / levers tried: Retail keeps D_800E27EC in $v1 (adjusted in place) with the quotient in $v0 and D_800F336A in $a0. cc1 does the adjust in $v0. Tried: `$3`/`$4`/`$2` pins on T, S, q (16-68), an explicit `if (T<0) T += 3; T >> 2` (28), a barrier on T (16), operand swap (28).
- Measured with `tools/analysis/era_link_check.py` through `build/lanes/exeD/t.sh`; the diff-lines counts come from an aligned objdump diff.

## exeD3 round (2026-09-27)
- Not re-attacked beyond reading. The residual is the D_800E27EC/D_800F336A register pair in the /4 and /8 multiply expressions.
