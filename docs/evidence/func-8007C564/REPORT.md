# func_8007C564 — LINK_EXACT pending one maspsx knob (lane exeF, 2026-09-28)

- VRAM `0x8007C564`, file offset `0x6CD64`, size `0x91C` (583 words), `asm/disc1/6CD60.s`
  (the `[0x6CD60, asm]` row keeps the 4-byte `func_8007C560` nop filler).
- Drafted from the asm in this lane (no prior draft). CD streaming sector-ready callback:
  result bytes via `func_8007A488(1, res)`, 0x160 STR header check in the ring slot
  `D_800C0DC8 + D_800BE998 * 32`, status codes 1..10 into `D_8009B374`, DMA via
  `func_8007CE80` / `func_8007CEAC`, ring advance, `func_8007C214()` at end of frame.
- Profile (new) `era_o2_g0_nosr_c564`: stock cc1 2.7.2 `-O2 -G0 -fno-strength-reduce`, env
  `MASPSX_FILL_CALL_DELAY_SLOT=1 MASPSX_BEQZ_SYM_STORE_DELAY=1 MASPSX_BEQZ_SYM_STORE_DELAY_ANY=1
  MASPSX_LOAD_DELAY_NOP_BEFORE_LOOP_LABEL=1`.
  `MASPSX_BEQZ_SYM_STORE_DELAY_ANY` does not exist in the live maspsx yet — knob request K-BEQZ-ANY
  in build/lanes/exeF/KNOB_REQUESTS.md (one-condition patch build/lanes/exeF/knob_beqz_any.diff).
- YAML rows: add `- [0x6CD64, c, func_8007C564]` after `- [0x6CD60, asm]` (next row `0x6D680` is
  `func_8007CE80`).

## Verification (dev maspsx copy with the knob)
```
ERA_LINK_CHECK_MASPSX=build/lanes/exeF/maspsx_dev/maspsx.py MASPSX_BEQZ_SYM_STORE_DELAY_ANY=1 \
MASPSX_BEQZ_SYM_STORE_DELAY=1 MASPSX_FILL_CALL_DELAY_SLOT=1 MASPSX_LOAD_DELAY_NOP_BEFORE_LOOP_LABEL=1 \
  python3 tools/analysis/era_link_check.py src/func_8007C564.c 0x8007C564 0x91C -O2 -G0 -fno-strength-reduce
  linked .text 2336 bytes, target 0x91c, word mismatches=0, nonzero_pad=0
  LINK_EXACT
```
Deep preflight / LEAF_STRONG must be re-run once the knob is in the live maspsx (with the live
tree today the deep preflight reports 559 mismatches — the 4 unfilled beqz slots shift the body).

## Load-bearing pieces (drop-one ladder, all else on)
- `MASPSX_BEQZ_SYM_STORE_DELAY` / `_ANY`: 559 each (4 `beqz` slots with `sw %lo(sym)($at)`).
- `MASPSX_FILL_CALL_DELAY_SLOT`: 537 (sym stores in `j .L8007CE70` slots).
- `MASPSX_LOAD_DELAY_NOP_BEFORE_LOOP_LABEL`: 1 (loop-2 branch offset).
- `-fno-strength-reduce`: 471 (retail keeps index counters in both copy loops).
- `MASPSX_FILL_JUMP_REG_STORE`: not needed.
C levers (aligned-diff ladder 96 → 0): unsized `volatile unsigned short st[3]` (retail's 0x22/0x24
slots); `D_800A34A0` non-volatile + `*(volatile unsigned short *)p;` for retail's dead `lhu` in the
header-mismatch else arm; `mode = 0x11000000` before the first DMA if (lui t0 in the beqz slot);
direct `D_800A34A0[k]` reads in the two late tests (52 → 42); `asm("")` at the head of the
post-check block (reorg stop_search_p: keeps the `lui` of 0x20943 out of the beqz slot, 23 → 21);
`unsigned int *q = D_8009B33C;` before the 4-byte header struct copy (21 → 9); `i = 0` hoisted
above `if (D_800A8020 == 0)` so reorg fills that `bnez` slot from before (→ 1 → 0 with the
loop-label nop knob).
