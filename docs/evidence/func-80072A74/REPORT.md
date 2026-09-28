# func_80072A74 (__adddf3) — EXACT (lane exeD6, 2026-09-28)

VRAM `0x80072A74`, file `0x63274`, size `0x374`. Psy-Q soft-float __adddf3.
YAML: `[0x63250, asm]` then `- [0x63274, c, func_80072A74]` and `- [0x635E8, asm]` (pad before `func_80072DF4`).
Profile `era_o2_g0_fill_epilogue_delay_slot_branch_steal_target_insn_fill_jump_preceding_move_load_delay_nop_before_any_label`:
cc1 2.7.2 `-O2 -G0` + `MASPSX_FILL_EPILOGUE_DELAY_SLOT=1 MASPSX_BRANCH_STEAL_TARGET_INSN=1
MASPSX_FILL_JUMP_PRECEDING_MOVE=1 MASPSX_LOAD_DELAY_NOP_BEFORE_ANY_LABEL=1`. Source `src/func_80072A74.c` (new; there
was no prior draft).
New maspsx patches (tests in tools/era/maspsx/tests/):
- 45 BRANCH_STEAL_TARGET_INSN: retail reorg steals a target insn that writes the tested register, and moves it when
  the fall-through already ran the slot.
- 46 FILL_JUMP_PRECEDING_MOVE: ASPSX fill of a reorder-mode `j` with the preceding move.
- 47 LOAD_DELAY_NOP_BEFORE_ANY_LABEL: patch 14 without its directive-block condition.
Knob-drop ladder: all → LINK_EXACT; without EPILOGUE 2, STEAL_TARGET 191, JMOVE 181, NOP_BEFORE_ANY_LABEL 3.

## Structure and levers (first draft 45 → 0 aligned lines)
- The result is built in the `a` union (retail `$18/$19` = a's copies), and the early exits return a/b.
- The volatile-looking zero double at sp+48 is a plain `DU z` forced to memory. `__asm__("" :: "m"(z.w.lo))` after
  `z.w.lo = 0` puts that store first. `__asm__("" : "=m"(z))` before `return z.d` forces the reload, and the second
  store lands in the `bne` slot.
- Retail keeps constants in callee-saved regs:
  - `0x80000000` as `register ... hb asm("$21")`, kept live by `__asm__("" :: "r"(hb))` inside the b-negate arm.
  - the loop's `0xE0000000` as a `$16`-pinned local in a scope around the normalize loop.
  - `0x40000000` as a `$16`-pinned local around the two round-carry tests.
- The rounding value is a local `n` (0xFF, or 0x100 if `ma.lo & 0x200`), stored after `rnd.hi = 0`. The overflow high
  word goes through a temp `h` with `sign != 0` first.

## Verification (shared tools/era/maspsx with patches 44–47 landed; private config build/lanes/exeD6/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD6/disc1.yaml --profiles build/lanes/exeD6/disc1_build_profiles.json --deep --only func_80072A74
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD6/disc1.yaml --profiles build/lanes/exeD6/disc1_build_profiles.json func_80072A74
func_80072A74 | disc1_preflight: PASS (deep, 2134 c / 154 asm / 2 rodata) | LEAF_STRONG=EXACT func_80072A74 span=0x374 vram=0x80072A74 compiled=0x380 failures=[none]
```
Flag-off identity of patches 44–47: `maspsx_knob_sweep.py --default-check` 4789/4789 IDENT (build/lanes/exeD6/sweep/ALL.log).
