# func_8001C7DC — EXACT (lane exeA6, 2026-09-28)

- VRAM `0x8001C7DC`, file offset `0xCFDC`, size `0x2D4` (ends at the existing `[0xD2B0, c, func_8001CAB0]` row)
- YAML row: `- [0xCFDC, c, func_8001C7DC]` (replaces `- [0xCFDC, asm]`; no resume row needed)
- Profile: `era_o2_g8_expand_div_force_d8009d2e8_absolute` = era `-O2 -G8`,
  `MASPSX_EXPAND_DIV=1`, `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2E8` (both knobs load-bearing:
  without EXPAND_DIV 108 word mismatches; without the force 158)
- Source: `src/func_8001C7DC.c`. Walk-mesh wall-distance search (sibling of `func_8001CBA0`):
  project, perpendicular distance to the current wall, then an outward `k` search in 4 axis directions.

## Verification (private config build/lanes/exeA6/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA6/disc1.yaml --profiles build/lanes/exeA6/disc1_build_profiles.json --deep --only func_8001C7DC
  disc1_preflight: PASS (deep, 2111 c / 171 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA6/disc1.yaml --profiles build/lanes/exeA6/disc1_build_profiles.json func_8001C7DC
  LEAF_STRONG=EXACT func_8001C7DC span=0x2D4 vram=0x8001C7DC compiled=0x2E0 failures=[none]
tools/analysis/era_link_check.py src/func_8001C7DC.c 0x8001C7DC 0x2D4 -O2 -G8   (env as profile)
  linked .text 736 bytes, target 0x2d4, word mismatches=0, nonzero_pad=0
```
`profile_necessity.py` has no `--config`; run after integration.

## Load-bearing levers (aligned-word ladder, build/lanes/exeA6/al.sh)
1. **loop.c threshold decay** (new, durable): `move_movables` does `threshold -= 3` after every moved
   insn (confirmed in gcc-2.9x `loop.c`, behaviour matches the `-dL` dumps). An invariant chain late in the
   loop is moved only if `threshold*savings*lifetime >= insn_count`. Retail keeps the wall-address chain
   (`idx*12 + base`) inside the `j` loop; cc1 moved it with 13 prior moves. Writing the per-k `dx` as
   `xb - (short)(xa >> 16)` adds two movables (sign-extend pair, later folded away by combine) before the
   chain so it becomes "not desirable" (`-dL`: `Insn …: cond savings 2 not desirable`). 108 → 115.
2. `k` loop as `goto kloop` (not `for (;;)`, which loop.c processes and hoists the per-k reloads out of),
   with the in-loop found block reached by `goto found` placed textually between the first test and the
   loop (`goto start; found: …; return; start:`) — reproduces retail's block layout (found blocks before
   the loop) and `k = 0` in the `beq` delay slot. Block-local copies `int xb = x1; int xa = x0;` before
   the `kloop` label give retail's `$3=$7` / `$4=$8` preheader copies. 108 → 114.
3. Pre-loop `dx = x1 - (x0 >> 16);` and `dz = D_8009CE12 - D_8009CE0E;` as statements before the distance
   (operand order `(pz part) * dx`). 115 → 137.
4. `d = D_8009CE14[D_8009CE18].len; d = num / d;` (divisor lives in `d`'s `$16`, as retail). In the loop
   the numerator goes to a block temp first (`int t = …; d = len; d = t / d;`) so the len load stays late
   (keeps the chain non-desirable). 137 → 141 / 138.
5. Separate loop variables `dx2`/`dz2` (sharing `dx` with the pre-loop made reload spill `$15` and shift
   every loop register by one). 141 → 162; placing `dx2`/`dz2` after the switch fixes preheader order → 164.
6. Global-alloc priority: three `asm("" : : "r"(pz))` after `pz` is computed and one `asm("" : : "r"(px))`
   after `px += o->f40` give retail's `px=$19, pz=$20, o=$21` (2 pz uses: unchanged; 4 pz + 1 px: also
   exact; 3 pz + 0 px: 167). 164 → 181/181, 0 word mismatches.
