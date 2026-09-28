# func_8001B5FC — PARKED (lane exeA, 2026-09-27, round 2)

- VRAM `0x8001B5FC`, file offset `0xBDFC`, size `0xB68`
- Best: 492/718 aligned (lnk 745 words)
- Profile: -O2 -G8, MASPSX_EXPAND_DIV=1, MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009CD88
- Best draft: `build/lanes/exeA/drafts/func_8001B5FC.c` (not in `src/`)
- Alignment metric: `build/lanes/exeA/al.sh <draft> 0x8001B5FC 0xB68 "<env>" -O2 -G8`. This dumps the linked `.text` via `ERA_LINK_CHECK_DUMP`, runs objdump on retail and link, and does a difflib opcode alignment with branch targets normalised.

## Status

Recursive walk-mesh wall search over two mesh formats (`D_8009D1D8` selects 22-byte triangles with 4-byte vertices or 28-byte triangles with 6-byte vertices), `D_8009DFB0` edge-visited bitmask, 3-neighbour recursion. The 6-byte `{-1,-1,-1}` initializer at `D_8009CD88` sits in the `$gp` window, so `MASPSX_RODATA_FOLD` (which wants `.rdata`) cannot fold it. It is written as a struct copy `S3 list = D_8009CD88;` (retail `lwl/lwr/lh`). Block-scoped per-mode locals reproduce retail's separate `X/Z/tp/e/k` spill slots. Residual: loop.c strength-reduces `e[7]` into its own pointer giv (retail keeps plain `e` offsets from the spilled `e`), and the spill-slot layout/frame (`0xC0` vs `0xD0`).
