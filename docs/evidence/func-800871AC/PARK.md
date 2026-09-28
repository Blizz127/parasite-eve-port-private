# func_800871AC — PARKED (lane exeD, 2026-09-27)

VRAM `0x800871AC`, file `0x779ac`, size `0x268`, `asm/disc1/779AC.s`.

- Best: 13 words under 2.8.1 + MASPSX_UNFILL_EPILOGUE_DELAY_SLOT=1 (13 words / 17 diff lines); archived 2.7.2 also 13.
- Draft: `build/lanes/exeD/drafts/func_800871AC.c`.
- Residual / levers tried: Three regions: (1) retail `addiu v0,s2,-64; subu s2,v0,s1; addu s1,s2,zero` then `sltu v0,s1,s5`, i.e. `chunk = a1; if (chunk >= size) chunk = size` with a1 kept in $s2. Every `chunk = a1` spelling tried (6 forms, plus barriers on a1 and on a `t = a1 - 0x40` temp) reaches 10/9 diff lines, but cc1 coalesces the copy (or reassociates `a1 - (chunk + 64)`). (2) `ori s2,zero,0x8000` stolen into a second branch slot. (3) the tone-table copy loop counter/pointer register pair (s1/v1 vs v1/a0).
- Measured with `tools/analysis/era_link_check.py` through `build/lanes/exeD/t.sh`; the diff-lines counts come from an aligned objdump diff.
