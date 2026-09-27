# func_80032B0C — PARKED (lane exeB, 2026-09-27)

- VRAM `0x80032B0C`, file offset `0x2330C`, size `0x850`; damage-number sprite emitter
  (`D_800B01C0[D_8009CDDC][D_8009D230][n]` SPRT records, digit loop, colour switch).
- Profile tried: `-O2 -G8` + `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009CDDC`.
- Best draft: `build/lanes/exeB/drafts/func_80032B0C.c` (unchanged prior draft), 514 mismatches
  (linked 2144 vs 2128 bytes).

## Residual (new evidence this lane)
1. **Index evaluation order.** For every `E2` store, retail loads `D_8009CDDC` first
   (absolute `lui/lw`, then `*1400` via `sll 1/addu/sll 2/subu/sll 4/subu/sll 3`), then
   `D_8009D230` (gp, `*140`). cc1 expands `D_800B01C0[i][j]` with the inner index `j`
   first (`get_inner_reference` builds `j*140 + i*1400`).
2. **Frame size changes with the index spelling.** Every rewrite that would put `i` first
   raises the frame from 104 to 144 bytes (40 unused bytes between `buf[48]` and the register
   save area): `((Ent (*)[5])D_800B01C0[i])[j]`, `*(D_800B01C0[i] + j)`,
   `(Ent *)((char *)D_800B01C0 + i*1400 + j*140)`, `(Ent *)(... + i*1400) + j*5`,
   `&D_800B01C0[i][0][0] + j*5`, `(*(D_800B01C0 + i))[j]`, `(D_800B01C0 + i)[0][j]`. All
   give 518. They look like reload stack slots for pseudos that never got a hard register
   and were later replaced by their equivalences.
3. A sized alias `extern int D_8009CDDC_arr[4] asm("D_8009CDDC")` (so cc1 sees it as
   non-small-data) makes cse keep the address in a register (`lui/addiu`, `lw 0($a1)`):
   499 mismatches, frame 136.
Next idea: find a spelling that expands `i` first without the extra pseudos — for example a
nested struct `{ Ent e[5]; } row[10]` with `COMPONENT_REF`, or an index computed into a
block-local in the macro.
