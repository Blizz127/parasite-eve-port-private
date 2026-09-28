# func_80072A74 — PARKED (lane exeD, 2026-09-27)

VRAM `0x80072A74`, file `0x63274`, size `0x374`, `asm/disc1/63250.s`.

- Best: not drafted under -.
- Draft: `build/lanes/exeD/drafts/func_80072A74.c` (if present).
- __adddf3: the same Psy-Q soft-float family as func_800735C4, not fp-bit.c. Calls func_80072EA4 (arithmetic shift) x7, func_80073554 (negate) x3, func_80072DF4 (add64) x2, func_800739C4 x1; no pack/unpack. The volatile local is zeroed in the prologue (`sw $zero,0x30($sp)`). Compiled C; expect the __muldf3 allocation class.
