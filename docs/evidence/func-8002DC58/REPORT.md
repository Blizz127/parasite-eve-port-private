# func_8002DC58 — scripted battle exit step (mode 8)

- VRAM `0x8002DC58`, file offset `0x1E458`, size `0x1458` (ends on `jr $ra` at `0x8002F0A8`+delay; next row `0x1F8B0` = `func_8002F0B0`).
- Profile `era_o2_g8_hud_colour_block_tail1_0158` = `era_o2_g8_hud_colour_block` env + `MASPSX_HOIST_BLOCK_LI_TAIL1=D_800B0158`
  (`-O2 -G8`, `MASPSX_THREE_WORD_SYMBOL_STORE=1`, `MASPSX_HOIST_BLOCK_LI=D_8009CDDC`, same FORCE_ABSOLUTE list).
- yaml: `- [0x1E458, asm]` → `- [0x1E458, c, func_8002DC58]` (no resume row needed).
- Semantic guide: `pc_port/game/boot/func_8002DC58_port.c` (AT/PE pulse, command settle loop over the actor list,
  `D_8009CE74` 0/1 switch, mode-12 cleanup with default colour restore).

## Verification (lane exeB5 private config)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeB5/disc1.yaml --profiles build/lanes/exeB5/disc1_build_profiles.json --deep --only func_8002DC58
  disc1_preflight: PASS (deep, 2102 c / 178 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeB5/disc1.yaml --profiles build/lanes/exeB5/disc1_build_profiles.json func_8002DC58
  LEAF_STRONG=EXACT func_8002DC58 span=0x1458 vram=0x8002DC58 compiled=0x1460 failures=[none]
era_link_check (profile env, -O2 -G8): LINK_EXACT, word mismatches=0
```
profile_necessity.py has no `--config`; run after integration.

## Levers (mismatch ladder)
1. Per-field byte symbols (`D_800B00EC[D_8009CDDC * 36]`, `D_800B0158[.. * 72]`, `D_800B692C[.. * 28]`), func_8002D1F0 style;
   the mode-12 restore uses fixed `D_800B00ED[0]` stores in natural source order — cc1's scheduler produces retail's
   value-grouped order by itself.
2. **Cross-jump limit via asm-alias index symbols**: the last store of each of the four cases in each colour block
   reads `D_8009CDDC_a.._d` (`asm("D_8009CDDC")`). Without it cc1 cross-jumps whole identical cases
   (text 0xF20 vs 0x1458, 787 mismatches); with it only the final `sll/subu/sb` tail merges, as in retail.
3. `MASPSX_HOIST_BLOCK_LI_TAIL1=D_800B0158` for the stride-72 PE gauge block (size 0x1450 → 0x1458; 683 → 30).
4. Register pins (func_8002BC90's): block-1 cases 1/3 `0x50/0xA3/0xBE` in `$5/$6/$4` (30 → 6), case 2 `0x9F/0xFF`
   in `$7/$8` (6 → 0).
5. `if (!(D_8009D1A0 & 0x1800)) call(0x15); else { clear; call(0x18); }` — zero arm first gives retail's
   `bnez` + `li $a1,0x18` delay slot layout.
6. `switch (D_8009CE74)` with cases 0/1 (retail `beqz`/`beq 1`/`j end`); `func_8006D60C(0) == 1` reuses `$s0`.
