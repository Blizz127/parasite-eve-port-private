# func_8002B94C — PARKED (lane exeA, 2026-09-27)

- VRAM `0x8002B94C`, file offset `0x1C14C`, size `0x344`
- Best: 95 (unchanged from decompA)
- Profile tried: -O2 -G8, MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D20C,D_800B0D88,D_800B0D8A
- Best draft: `build/lanes/exeA/drafts/func_8002B94C.c` (not in `src/`)

## Residual

Two case-0 issues shift everything: (a) retail reloads `t->f98` after the `|= 0x1000`
store and tests the reload; cc1 forwards the stored value. (b) in the `t == D_8009D254` arm
retail loads `D_800B0D88` before `D_8009D254`.

## Levers tried

decompA: inline helper, local copy, volatile read. This lane: constant-address
`*(unsigned short *)0x800B0D88 |= 2` (fixes (b)'s order but loop.c hoists the `lui` into `$s1`,
171), pointer-local D88 (95), volatile D88 (95), volatile-cast `D_8009D254` read (176).

Measured with `tools/analysis/era_link_check.py <draft> 0x8002B94C 0x344 <flags>` (profile env in a subshell).
