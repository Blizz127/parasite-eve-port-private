# func_80084B78 — matching C leaf

- VRAM `0x80084B78`, ROM offset `0x75378`, span `0xD4`
- Source: `src/func_80084B78.c`; YAML rows `[0x75378, c, func_80084B78]` + `[0x7544C, asm]`
- Build profile: `era_o2_g0_no_delayed_branch_reorder_fill_calls`
  = cc1 2.7.2 `-O2 -G0 -fno-delayed-branch` + `MASPSX_REORDER_FILL_CALLS=1` (maspsx patch 22)

## History
- decompB draft: control flow and register allocation exact; residual 21 words, all
  delay-slot placement. Retail was assembled in ASPSX reorder mode: cc1 leaves every
  slot empty (`-fno-delayed-branch`) and the assembler fills only the `jal` slots
  and the epilogue `j $31` slot. Branch and `j` slots stay `nop`. No existing knob did this
  (patch 8 only fills a macro's second word; patch 3 only the `addiu $sp` epilogue).
- tooling2 built patch 22 (`MASPSX_REORDER_FILL_CALLS`):
  - `X; jal T` becomes `jal T; X` when X is the single-word ALU op right before the jal
    (no loads, stores, branches, macros, `$ra`/`$sp`/`$at`, or jalr target; no label in between).
  - The epilogue `X; lw $31; addu $sp; j $31` becomes `lw $31; X; j $31; addu $sp`.
  - Corpus sweep with the knob on: 0 matched leaves change.

## Verification
```
ERA_LINK_CHECK_MASPSX=build/fanout/tooling2/m22/maspsx.py MASPSX_REORDER_FILL_CALLS=1 \
  python3 tools/analysis/era_link_check.py build/fanout/decompB/drafts/func_80084B78.c 0x80084B78 0xD4 -O2 -G0 -fno-delayed-branch
  -> LINK_EXACT   (tooling2, pre-install copy)
python3 tools/analysis/leaf_strong_check.py func_80084B78
  -> LEAF_STRONG=EXACT   (tooling2 / team-lead after installing patch 22 into tools/era/maspsx; merged in batch 10)
```
Draft: decompB. Patch 22 + registration: tooling2.
