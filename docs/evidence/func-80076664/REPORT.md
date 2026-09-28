# func_80076664 — EXACT (exeE2 draft, landed by lane exeD6, 2026-09-28)

GPU image upload/read-back twin (func_80076664 / func_800768A0). YAML `[0x66E64, c, func_80076664]`.
Profile `era_o2_g0_cc1_281_swap_s1_s2_save_pairs_rev_symbol_pointer_load_li_after`: cc1 2.8.1 `-O2 -G0 -mno-split-addresses`.
Env: `MASPSX_SWAP_S1_S2_SAVE_PAIRS_REV=1 MASPSX_SYMBOL_POINTER_LOAD_LI_AFTER=1`.
Source: `src/func_80076664.c` (exeE2's `build/lanes/exeE2/drafts/func_80076664.best.c`).

New maspsx patches (this lane):
- 38 `SWAP_S1_S2_SAVE_PAIRS_REV` is the reverse of patch 26. Retail saves the $s1=$a0 pair before the $s2=$a1 pair.
- 39 `SYMBOL_POINTER_LOAD_LI_AFTER` rewrites `lw $r,SYM / li $k,K / lw $d,0($r)` to load, load, li, with a
  preceding independent `move` moved into the first load delay.

Knob-drop ladder (real maspsx): both → LINK_EXACT; either one alone → mismatch (prologue save order / poll-load
schedule); neither → mismatch. Tests: `test_swap_s1_s2_save_pairs_rev.py`, `test_symbol_pointer_load_li_after.py`.

## Verification
```
func_80076664 | disc1_preflight: PASS (deep, 2116 c / 167 asm / 2 rodata)  | LEAF_STRONG=EXACT func_80076664 span=0x23C vram=0x80076664 compiled=0x240 failures=[none]
```
