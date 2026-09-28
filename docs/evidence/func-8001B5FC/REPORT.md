# func_8001B5FC — EXACT (lane exeA6, 2026-09-28)

- VRAM `0x8001B5FC`, file offset `0xBDFC`, size `0xB68` (718 words; ends at the existing `[0xC964, c, func_8001C164]` row)
- YAML row: `- [0xBDFC, c, func_8001B5FC]` inserted after `- [0xB640, asm]` (no resume row needed)
- Profile: existing `era_o2_g8_expand_div` (era `-O2 -G8`, `MASPSX_EXPAND_DIV=1`). The exeA PARK's extra
  `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009CD88` is not needed (with it alone and no EXPAND_DIV: 572 mismatches;
  EXPAND_DIV alone: 0).
- Source: `src/func_8001B5FC.c`. Recursive walk-mesh wall search over the two mesh formats
  (`D_8009D1D8` selects 22-byte/4-byte-vertex or 28-byte/6-byte-vertex triangles), `D_8009DFB0` edge bitmask.

## Verification (private config build/lanes/exeA6/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA6/disc1.yaml --profiles build/lanes/exeA6/disc1_build_profiles.json --deep --only func_8001B5FC
  disc1_preflight: PASS (deep, 2112 c / 171 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA6/disc1.yaml --profiles build/lanes/exeA6/disc1_build_profiles.json func_8001B5FC
  LEAF_STRONG=EXACT func_8001B5FC span=0xB68 vram=0x8001B5FC compiled=0xB70 failures=[none]
tools/analysis/era_link_check.py src/func_8001B5FC.c 0x8001B5FC 0xB68 -O2 -G8   (MASPSX_EXPAND_DIV=1)
  linked .text 2928 bytes, target 0xb68, word mismatches=0, nonzero_pad=0
```
`profile_necessity.py` has no `--config`; run after integration.

## Load-bearing levers (aligned-word ladder from exeA's 492/718)
1. **Counter loop, not a pointer walk**: `k = 0; do { … t->v[k] / t->w[k] / t->nb[k] … k++; } while (k < 3);`
   Retail's `e` pointer (spilled at 136) and the `2k` offset (144) are loop.c's reduced givs and the exit
   test `e < tp + 6` is the eliminated biv; an explicit `e++` walk made loop.c reduce `e[7]` into its own
   giv (`-6($9)` offsets). 492 → 548 (frame 0xD0 → 0xC0).
2. Stack-slot order = pseudo creation order: `void *tp = t;` declared first in each mode block, and a separate
   `int w` declared first at function level (spilled with `sw` at 40) with a short-lived `unsigned short w0`
   for the bitmask. 548 → 613.
3. `d = (short)(D_8009CE14[w].d >> 16); d = num / d;` — divisor in `d`'s `$16` (as func_8001C7DC). 613 → 633.
4. `unsigned int k` (exit test `sltu`). 633 → 635.
5. Global-alloc priority for the recursion loop: `asm("" : : "r"(i));` at the top of the mode-0 loop body and
   `i = 0; asm("" : : "r"(i)); for (; i < 3; i++)` in mode 1 (an in-loop asm in mode 1 changes loop.c's
   hoist of `x << 16`) → `i=$16`, `&list.a[i]` giv `$17`. 635 → 647.
6. Bitmask word/mask as locals right after the `w0` load: `p = &D_8009DFB0[w0 >> 5]; m = 1 << (w0 & 0x1F);`
   then `if (*p & m) …; *p |= m;` — sched1 order of the loop head. 647 → 694.
7. Comparison operand order in the wall-corner min/max: `if (px > ox)` / `if (pz > oz)` (not `ox < px`)
   gives retail's temp registers and the unfilled `beq` delay slot. 694 → 718/718, 0 word mismatches.
