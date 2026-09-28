# func_800735C4 (__muldf3) — EXACT (lane exeD6, 2026-09-28)

VRAM `0x800735C4`, file `0x63DC4`, size `0x314`. Psy-Q soft-float __muldf3 (compiled C over the S64 helpers).
YAML `- [0x63DC4, c, func_800735C4]` (replaces `- [0x63DC4, asm]`; next row `[0x640D8, c, func_800738D8]`).
Profile `era_o2_g0_fill_jump_stack_store_fill_epilogue_delay_slot_sink_volatile_stack_load`:
cc1 2.7.2 `-O2 -G0` + `MASPSX_FILL_JUMP_STACK_STORE=1 MASPSX_FILL_EPILOGUE_DELAY_SLOT=1 MASPSX_SINK_VOLATILE_STACK_LOAD=1`.
Source `src/func_800735C4.c`. New maspsx patch 44 `SINK_VOLATILE_STACK_LOAD` (test `tests/test_sink_volatile_stack_load.py`).
Knob-drop ladder (word mismatches): all three → LINK_EXACT; without FILL_JUMP_STACK_STORE 167, without
FILL_EPILOGUE_DELAY_SLOT 2, without SINK_VOLATILE_STACK_LOAD 4.

## Levers (exeD draft 68 → 0 aligned lines)
1. `x = v - 0x3FE; e = v;` from a local sum. `x = (e = ...) - 0x3FE` re-reads the volatile e (68 → 36).
2. Patches 11 and 3 for the volatile `r` j-slot stores and the epilogue `addiu $sp` (36 → 30).
3. `rnd.hi = 0;` before `rnd.lo`. The overflow select is written `if (sign != 0) 0xFFF00000 else 0x7FF00000` (30 → 24).
4. The `0xFFFFF` mantissa mask is a `$3`-pinned local, with the exponent sum as `$3`-pinned `ea`/`v`. This gives retail's
   `sw e` before `lui $3,0xf` and the `and $2,$9,$3` operand order (24 → 2).
5. `0x80000000` goes through an asm output at the head of the else arm (`hb`, stays in `$s0` across the add call). Reorg
   then steals its `lui` into the `bgez` slot (24 → 16 on the way).
6. `((volatile S64 *)&acc)->lo` and the e reload as `$11`-pinned locals. Retail's `$11` loads are reload registers of
   pseudos it spilled; the volatile locals emulate the spill (16 → 2).
7. Patch 44 sinks the volatile `lw $11,80($sp)` to one insn before its use. This is where sched2 puts a spilled
   pseudo's reload: between the two argument-half loads (2 → 0).

## Verification (shared tools/era/maspsx with patches 44–47 landed; private config build/lanes/exeD6/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD6/disc1.yaml --profiles build/lanes/exeD6/disc1_build_profiles.json --deep --only func_800735C4
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD6/disc1.yaml --profiles build/lanes/exeD6/disc1_build_profiles.json func_800735C4
func_800735C4 | disc1_preflight: PASS (deep, 2134 c / 154 asm / 2 rodata) | LEAF_STRONG=EXACT func_800735C4 span=0x314 vram=0x800735C4 compiled=0x320 failures=[none]
```
Flag-off identity of patches 44–47: `maspsx_knob_sweep.py --default-check` 4789/4789 IDENT (build/lanes/exeD6/sweep/ALL.log).
