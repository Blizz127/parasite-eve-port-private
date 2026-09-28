# func_8008486C — EXACT (lane exeD6, 2026-09-28)

VRAM `0x8008486C`, file `0x7506C`, size `0x27C` (void; pad input grant loop).
Profile `era_o2_g0_cc1_281_fill_call_delay_slot_reload_cse_const_fill_branch_tested_li_void`:
cc1 2.8.1 `-O2 -G0 -mno-split-addresses` + `MASPSX_FILL_CALL_DELAY_SLOT=1 MASPSX_RELOAD_CSE_CONST=1
MASPSX_FILL_BRANCH_TESTED_LI_VOID=1` (maspsx patches 40 and 42 land with this lane).
YAML: `- [0x7506C, c, func_8008486C]` (replaces `- [0x7506C, asm]`; next row `[0x752E8, c, func_80084AE8]` unchanged).
Source: `src/func_8008486C.c` (= `build/lanes/exeD6/drafts/func_8008486C.final.c`). The link is 0x280 with one zero gas pad word:
`linked .text 640 bytes, target 0x27c, word mismatches=0, nonzero_pad=0` → LINK_EXACT.

## Verification
Before landing, with the private maspsx (`ERA_LINK_CHECK_MASPSX=build/lanes/exeD6/maspsx/maspsx.py`): LINK_EXACT.
Knob-drop ladder: without FILL_CALL_DELAY_SLOT 97; without RELOAD_CSE_CONST 2; without FILL_BRANCH_TESTED_LI_VOID 38.
2.7.2 is 28 and 2.8.1 without `-mno-split-addresses` is 128.

## Residuals closed (the PARK list)
- (a) `addu $10,$8,$0`, the outer giv init. It is 2.8.1's post-reload reload_cse (exeD5 diagnosis): `$8 = 0` from the
  guard's `move $8,$0` slot. RELOAD_CSE_CONST turns it back into `move $10,$0`.
- The tail order v0/v1/a0. A block-local `int v = 1; j = 5; u = a0 + 5;` gives retail's `li $2,1` / `li $3,5` order,
  and RELOAD_CSE_CONST undoes the `addu $4,$16,$3` (`$3 == 5`) back to `addiu $4,$16,5`.
- (b)/(c) The two `li $2,1` fills into `beq/bne $2,…,<epilogue>`. Retail's reorg fills the slot with an insn that writes
  the branch-tested register. cc1's reorg refuses (the trial sets a resource the branch needs). FILL_BRANCH_TESTED_LI_VOID
  fills it only when the `li` destination is tested by the branch and is dead at the target, counting the void return as
  a kill of caller-saved registers.

## Post-landing verification (shared tools/era/maspsx, private config build/lanes/exeD6/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD6/disc1.yaml --profiles build/lanes/exeD6/disc1_build_profiles.json --deep --only func_8008486C
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD6/disc1.yaml --profiles build/lanes/exeD6/disc1_build_profiles.json func_8008486C
func_8008486C | disc1_preflight: PASS (deep, 2116 c / 167 asm / 2 rodata)  | LEAF_STRONG=EXACT func_8008486C span=0x27C vram=0x8008486C compiled=0x280 failures=[none]
```
