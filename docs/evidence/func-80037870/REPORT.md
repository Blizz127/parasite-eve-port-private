# func_80037870 — EXACT (lane exeB6, 2026-09-28)

Message/dialogue glyph renderer (4 `D_800BCEA8` Pop records, TXT stream parser, DR_MODE+SPRT glyph packets).

- VRAM `0x80037870`, file offset `0x28070`, size `0x10A0` (1064 words); ends at `0x29110` = `func_80038910`
  (existing c row, so no resume row).
- Source `src/func_80037870.c` (best exeB5 draft + the levers below).
- Profile `era_o2_g8_dispatch_80010e6c_force_d8009cddc_d8009d1f4_d8009169d_absolute`:
  `-O2 -G8`, `MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_DISPATCH_FOLD=jtbl_80010E6C
  MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009CDDC,D_8009D1F4,D_8009169D`.
- YAML: `- [0x28070, c, func_80037870]` (replaces `- [0x28070, asm]`).

## Verification (private config build/lanes/exeB6/)
- `python3 tools/build/disc1_preflight.py --config build/lanes/exeB6/disc1.yaml --profiles build/lanes/exeB6/disc1_build_profiles.json --deep --only func_80037870`
  → `disc1_preflight: PASS (deep, 2111 c / 171 asm / 2 rodata)`
- `python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeB6/disc1.yaml --profiles build/lanes/exeB6/disc1_build_profiles.json func_80037870`
  → `LEAF_STRONG=EXACT func_80037870 span=0x10A0 vram=0x80037870 compiled=0x10A0 failures=[none]`
- `profile_necessity.py` has no `--config` (it reads the main yaml, so it rejects the private assignment as stale).
  Manual knob necessity with `tools/analysis/era_link_check.py src/func_80037870.c 0x80037870 0x10A0 -O2 -G8`:
  full env → 0 mismatches; without THREE_WORD → 2; without DISPATCH_FOLD → 2; without FORCE_ABSOLUTE → LD FAILED;
  `-G0` → 1055 (+size).

## Root cause of the old 803-word residual (gcc 2.7.2 loop.c)
The signed `/21` magic `0x30C30C31` was hoisted out of the glyph loop, then re-emitted by `emit_move_insn`.
sched1 splits that into lui/ori, so the pseudo has two sets, loses its REG_EQUIV and takes a stack slot (retail
rematerialises it per block). In 2.7.2 `move_movables`, `insn_count *= 2` happens for **every** already-moved
(`moved_once`) movable and is never undone. So each inner loop that moved an invariant earlier in the glyph
loop doubles the bar for every later movable. With threshold 29 (25 GPR + hi/lo/hilo non-fixed) − 4 moves = 17,
the signed group (3 matched blocks, savings 3, life 51) needs `insn_count > 2601`. Two doublings of ~813 get there.

## Load-bearing levers (mismatch ladder)
1. **FB-6 / FB-7 as real loops** whose invariants loop.c can move (`6` in FB-6, the `0xFFF0FFFF` mask in both).
   Each gives a `moved_once` doubling, and the signed magic stays block-local. The mask becomes a literal
   constant, not the exeB5 `mask` variable: loop.c hoists it out of every loop, sched1 splits it, and the spilled
   copy is exactly retail's `0x60($sp)` slot. State init is `rec->fl.b16 = 0;`. 803 → 634.
2. **Declare `ot, digits, stop, left, rec, s, font, …` inside the record `for` body.** A combine-deleted entry
   temp (`&D_800B0E38[...]` address, pseudo 96) still gets a stack slot (reg_n_refs from flow). Slots go in
   regno order, so retail's unreferenced slot 0x38 is that temp, numbered before the loop-body locals. → 607.
3. **`ctl++` inside the loop exit**: `for (;;) { if (hit) { ...; continue; } ...; ctl++; break; }` for both
   FB-6 and FB-7, and the else arms end with their own `ctl++` (cross-jumped to one tail). ctl is then set inside
   the loop, so `zext(ctl)` / `ctl+1` stay in the loop (retail reloads `lbu 40($sp)` each iteration). This also
   gives retail's top-test layout with the exit code after the back `j`. → 116.
4. `s = rec->f4;` before `digits = rec->dig[0];` (preheader store order) → 113.
5. FB default: `row = (... / 21) * 12; uv[1] = row; if (row >= 241) { uv[1] = row - 252; ... }` with
   `unsigned short row` (retail `move`/`sh`/`andi 0xffff`; the in-place form folds to `andi 0xfffc`).
6. case 9: `signed char sel = rec->fl.b22;` for the compare / `sel - 1` (unsigned char adds a copy) → 12.
7. FB-7 body `rec->fl.b16++;` (bitfield increment, not manual word RMW) → 1 (register homes in FB-6/7 fixed).
8. case 8: `*(digits + n - 1)` (retail `addu digits, n-1` operand order) → 0.

Rejected: `volatile ctl` (stops the zext hoist but forces a frame slot at 32 ahead of the spill slots).
