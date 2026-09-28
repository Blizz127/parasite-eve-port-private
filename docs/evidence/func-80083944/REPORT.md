# func_80083944 — EXACT (lane exeD6, 2026-09-28)

VRAM `0x80083944`, file `0x74144`, size `0x274` (libpad payload state machine: case 2 halfword table, case 3 5-byte
records, case 4 8-byte records plus a byte-copy loop through `D_800A5AD0`).
Profile `era_o2_g0_cc1_281_return_via_epilogue_jump_shift_count_const_reload_cse_const_branch_steal_jump_slot`:
cc1 2.8.1 `-O2 -G0 -mno-split-addresses` + `MASPSX_RETURN_VIA_EPILOGUE_JUMP=1 MASPSX_SHIFT_COUNT_CONST=1
MASPSX_RELOAD_CSE_CONST=1 MASPSX_BRANCH_STEAL_JUMP_SLOT=1` (maspsx patches 37, 40 and 41 land with this lane).
YAML: `- [0x74144, c, func_80083944]` (replaces `- [0x74144, asm]`; next row `[0x743B8, c, func_80083BB8]` unchanged).
Source: `src/func_80083944.c` (= `build/lanes/exeD6/drafts/func_80083944.final.c`).

## Verification
Before landing, with the private maspsx at `build/lanes/exeD6/maspsx` (`ERA_LINK_CHECK_MASPSX`):
`tools/analysis/era_link_check.py src/func_80083944.c 0x80083944 0x274 -O2 -G0 -mno-split-addresses` → LINK_EXACT.
(The post-landing preflight/leaf_strong lines are in the section below once added.)

Knob-drop ladder (word mismatches): all four = 0; without RETURN_VIA_EPILOGUE_JUMP 6; without SHIFT_COUNT_CONST 1;
without RELOAD_CSE_CONST 1; without BRANCH_STEAL_JUMP_SLOT 4. Plain 2.7.2 `-O2 -G0` 50; 2.8.1 without
`-mno-split-addresses` 60.

## Levers (from the exeD draft at 120)
1. A struct typing of the payload record (`tab`/`rec5`/`rec8`/`src`/`state`/`idx`/`cnt`/`next`, and `n2/n3/n4` at 0xE3/0xE9/0xEA).
2. Case 3 reads `s = p->src` into the function-wide `s` first, so retail's early `lw 60($5)` is scheduled before the
   `idx*5` table address (the PARK residual). A case-local copy is 125; the shared `s` gives 43.
3. `return 1` via `goto one;` into the function's final `return 1;` gives retail's shared `li $2,1` block that falls into
   the epilogue (43 → 37).
4. `e[4] = w >> 7` with `asm("" : "=r"(w) : "0"(w))` on the byte. Otherwise cc1 knows it is non-negative and emits
   `srl`; retail has `sra` (37 → 36).
5. Case 4 uses its own `e4`/`s4` pointer locals (not the case-3 `e`/`s`). Global alloc then gives retail's `$7`/`$6`
   homes (26 → 6), and the loop-exit test is inverted (`if (++idx >= n4) {…; return 0;} cnt = 0; return 0;`).
6. `*D_800A5AD0++ = *s4++;` before `n--; p->cnt--;` in the copy loop places `s++` in the `lbu` delay (6 → 4).
7. Three assembler levers for cc1 2.8.1 artefacts:
   * SHIFT_COUNT_CONST: `sll $2,$2,$4` with `$4 = 3` in the `j` delay slot becomes `sll $2,$2,3`.
   * RELOAD_CSE_CONST: `move $8,$2` with `$2 = -1` becomes `li $8,-1`.
   * BRANCH_STEAL_JUMP_SLOT: the two `return 0` branches steal the `move $2,$0` of the patch-12 `j` block at their target
     and go straight to the final `jr`, as retail's reorg (steal_delay_list_from_target) did. cc1 cannot, because its
     target is a RETURN.

## Post-landing verification (shared tools/era/maspsx, private config build/lanes/exeD6/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD6/disc1.yaml --profiles build/lanes/exeD6/disc1_build_profiles.json --deep --only func_80083944
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD6/disc1.yaml --profiles build/lanes/exeD6/disc1_build_profiles.json func_80083944
func_80083944 | disc1_preflight: PASS (deep, 2116 c / 167 asm / 2 rodata)  | LEAF_STRONG=EXACT func_80083944 span=0x274 vram=0x80083944 compiled=0x280 failures=[none]
```
