# func_800299CC — PARKED (lane exeA, 2026-09-27, round 2)

- VRAM `0x800299CC`, file offset `0x1A1CC`, size `0x10CC`
- Best: 880/1075 aligned; every word after the colour blocks aligns
- Profile: -O2 -G8, MASPSX_THREE_WORD_SYMBOL_STORE=1, MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D254,D_8009D230,D_8009D250,D_8009CDDC,D_8009D1F4,D_8009D1F0,D_8009D1A0,D_8009D290,D_8009D294,D_8009D1AC,D_8009D20C,D_8009D235,D_8009D298,D_800B0CE6,D_8009D29A,D_8009D29C,D_8009D2E8,D_8009D1CE
- Best draft: `build/lanes/exeA/drafts/func_800299CC.c` (not in `src/`)
- Alignment metric: `build/lanes/exeA/al.sh <draft> 0x800299CC 0x10CC "<env>" -O2 -G8`. This dumps the linked `.text` via `ERA_LINK_CHECK_DUMP`, runs objdump on retail and link, and does a difflib opcode alignment with branch targets normalised.

## Status

Battle main tick. Levers that landed: `char pad[0x1A0]` frame, an asm-label alias `extern int D_8009CDDC_b asm("D_8009CDDC")` on the final store of colour block 3 (stops cc1 cross-jumping identical blocks 1/3 and leaves retail's shared-tail `j`), `if (x) f(0); else f(1);` for the `== 0` call argument, the `Ent.f18` union (`f1A` is the high half of the `0x18` word), and `D_8009D298/29A/29C` as sized `[1]` arrays (in-struct alias class; unsized arrays hoist `la`). Residual: the colour-block `li` placement (see `func_8001D340`). Tried: inline helper with constant args, block-local colour variables in all 6 declaration orders, `-fno-schedule-insns{,2}`, `-O1`.
