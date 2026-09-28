# func_8007F0C8 — EXACT (exeE2 draft, landed by lane exeD6, 2026-09-28)

VRAM `0x8007F0C8`, file `0x6F8C8`, size `0x350`. YAML `- [0x6F8C8, c, func_8007F0C8]` (next row `[0x6FC18, c, func_8007F418]`).
Profile `era_o2_g0_dispatch_80011c9c_cc1_281_three_word_symbol_store_symbol_load_dest_temp_ulw_load_delay_nop_shift_count_const`:
cc1 2.8.1 `-O2 -G0 -mno-split-addresses`. Env: `MASPSX_DISPATCH_FOLD=jtbl_80011C9C MASPSX_THREE_WORD_SYMBOL_STORE=1
MASPSX_SYMBOL_LOAD_DEST_TEMP=1 MASPSX_ULW_LOAD_DELAY_NOP=1 MASPSX_SHIFT_COUNT_CONST=1`.
Source: `src/func_8007F0C8.c` (exeE2's `build/lanes/exeE2/drafts/func_8007F0C8.best.c`).

New maspsx patches (this lane): 36 `MASPSX_ULW_LOAD_DELAY_NOP`, which keeps cc1's `#nop` after `ulw` as a real nop
(retail `lwl/lwr/nop/swl/swr`), and 37 `MASPSX_SHIFT_COUNT_CONST`, which undoes cc1 2.8.1's reload_cse
`sll $3,$2,$3` (`$3 == 1`) back to `sll $3,$2,1`. There are no const-count sllv anywhere in retail.
Knob-drop ladder (real maspsx): both → LINK_EXACT. ULW only → mismatch at 0x8007F3B4 (the sllv). SHIFT only → size shift from 0x8007F190.
Tests: `tools/era/maspsx/tests/test_ulw_load_delay_nop.py`, `test_shift_count_const.py`.

## Verification
```
func_8007F0C8 | disc1_preflight: PASS (deep, 2116 c / 167 asm / 2 rodata)  | LEAF_STRONG=EXACT func_8007F0C8 span=0x350 vram=0x8007F0C8 compiled=0x350 failures=[none]
```
(commands as in the brief, with `--config/--profiles build/lanes/exeD6/...`)
