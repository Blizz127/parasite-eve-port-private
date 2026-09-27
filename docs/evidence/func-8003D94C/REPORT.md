# func_8003D94C — EXACT (lane exeB4, 2026-09-27)

- VRAM `0x8003D94C`, file offset `0x2E14C`, size `0x298` (GT4/GT3 tpage + CLUT rebase over `a0->prims`).
- Profile: existing `era_o2_g0_no_rerun_cse_after_loop` (`-O2 -G0 -fno-rerun-cse-after-loop`).
- YAML: `- [0x2E14C, asm]` → `- [0x2E14C, c, func_8003D94C]` (next row `0x2E3E4` is already c).
- Source: `src/func_8003D94C.c` (copy in `build/lanes/exeB4/drafts/`).

## Verification (private lane config build/lanes/exeB4/)
- `python3 tools/analysis/era_link_check.py src/func_8003D94C.c 0x8003D94C 0x298 -O2 -G0 -fno-rerun-cse-after-loop`
  → `word mismatches=0`
- `python3 tools/build/disc1_preflight.py --config build/lanes/exeB4/disc1.yaml --profiles build/lanes/exeB4/disc1_build_profiles.json --deep --only func_8003D94C`
  → `disc1_preflight: PASS (deep, 2088 c / 190 asm / 2 rodata)`
- `python3 tools/analysis/leaf_strong_check.py --config ... --profiles ... func_8003D94C`
  → `LEAF_STRONG=EXACT func_8003D94C span=0x298 vram=0x8003D94C compiled=0x2A0 failures=[none]`
- `profile_necessity.py` has no `--config` (reads the main yaml), so run it after integration.
  Manual necessity: default `-O2 -G0` → 1 mismatch; `-O2 -G8` → 1; with `-fno-rerun-cse-after-loop` → 0.

## Levers (on top of the exeB2/exeB3 draft: ts/cc copy barriers, ptr ref barriers, for-increment ptr step)
1. **A cse1 block boundary with no code**: `cc = c; L: z = 0; if (z) goto L;`.
   The backward jump keeps label `L` alive into cse1, so the entry extended block ends
   at `L` and cse never records `cc ≡ c`. cse1 then folds the `if (z)` jump (z is the
   constant 0) and jump2 removes the label, so no code is emitted and sched/reorg put the
   `cc` copy back into the `beq` delay slot. Without it, cse's `make_regs_eqv` keeps the
   load pseudo as the class head on the entry→EQ path and rewrites the EQ arm's `cc` to `c`
   (the old 1-word residual `sll $2,$7,6` vs retail `sll $2,$10,6`).
2. **`-fno-rerun-cse-after-loop`**: by cse2 the label is gone and the equivalence comes
   back, so cse2 redoes the rewrite (1 mismatch under plain -O2).
3. **NE self-tie `asm("" : "=r"(c) : "0"(c));`** before `clut = c * 64 - 0x7080`: adds refs
   to the load pseudo so global alloc gives it `$7` and the copy `$10` (without it the homes
   swap: 4 mismatches).

Ladder: exeB3 best 1 (-fno-rerun) → +label boundary, no NE tie: 4 (homes swapped) → +NE self-tie: 0.
