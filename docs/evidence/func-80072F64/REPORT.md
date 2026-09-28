# func_80072F64 (__divdf3) — EXACT (lane exeD6, 2026-09-28)

VRAM `0x80072F64`, file `0x63764`, size `0x2E0`. Psy-Q soft-float __divdf3 (restoring division over a 64-bit bit mask).
YAML `- [0x63764, c, func_80072F64]` after `[0x63760, asm]` (next row `[0x63A44, c, func_80073244]`).
Profile `era_o2_g0_fill_epilogue_delay_slot_branch_steal_target_insn_any_load_delay_nop_before_any_label`:
cc1 2.7.2 `-O2 -G0` + `MASPSX_FILL_EPILOGUE_DELAY_SLOT=1 MASPSX_BRANCH_STEAL_TARGET_INSN=any
MASPSX_LOAD_DELAY_NOP_BEFORE_ANY_LABEL=1`. Source `src/func_80072F64.c`: plain C, no pins and no asm.
Knob-drop ladder: all → LINK_EXACT; without EPILOGUE 2, STEAL (or with mode `1`) 2, NOP_BEFORE_ANY_LABEL 3.

## Levers (first draft 63 → 0 aligned lines)
- `ma = func_80072DF4(ma, func_80073554(mb));` (negate nested in the add's arguments). The struct-return address lives
  long enough for loop.c to hoist `&ma` into `$21`, as retail does. A named `t` local is not hoisted.
- The rounding arms each end in their own `q = func_80073294(1, q, 1)` / `(1, q, n + 1)` call, and the post-reload
  cross-jump merges them as in retail. This makes the constant 1 a single-set pseudo (a sched1 birth), so it is placed
  after the `$4` struct address. It also yields retail's `$16`/`$17`/`$18-19` callee-saved assignment without pins.
  One shared tail call after the `if` gives 5–6.
- The divide-by-zero high word uses `sign != 0` first. `STEAL_TARGET_INSN=any` reproduces the `lui $2,0x7fff` stolen
  into the `bne $10` slot. That branch target has a second referrer, so the insn is copied and the branch goes to a
  label after it.

## Verification (shared tools/era/maspsx with patches 44–47 landed; private config build/lanes/exeD6/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD6/disc1.yaml --profiles build/lanes/exeD6/disc1_build_profiles.json --deep --only func_80072F64
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD6/disc1.yaml --profiles build/lanes/exeD6/disc1_build_profiles.json func_80072F64
func_80072F64 | disc1_preflight: PASS (deep, 2134 c / 154 asm / 2 rodata) | LEAF_STRONG=EXACT func_80072F64 span=0x2E0 vram=0x80072F64 compiled=0x2E0 failures=[none]
```
Flag-off identity of patches 44–47: `maspsx_knob_sweep.py --default-check` 4789/4789 IDENT (build/lanes/exeD6/sweep/ALL.log).
