# func_8002BC90 — PARKED (lane exeA, 2026-09-27, round 2)

- VRAM `0x8002BC90`, file offset `0x1C490`, size `0x1560`
- Best: 1128/1368 aligned (lnk 1348 words)
- Profile: same env as func_800299CC
- Best draft: `build/lanes/exeA/drafts/func_8002BC90.c` (not in `src/`)
- Alignment metric: `build/lanes/exeA/al.sh <draft> 0x8002BC90 0x1560 "<env>" -O2 -G8`. This dumps the linked `.text` via `ERA_LINK_CHECK_DUMP`, runs objdump on retail and link, and does a difflib opcode alignment with branch targets normalised.

## Status

Battle-end tick: the same HUD colour blocks plus a second gauge on `D_800B0130[].b`, the actor update loop, and a HUD reset. Same levers as `func_800299CC` (alias on block-3 final store, `Ent.f18` union, sized arrays, `char pad[0x300]`). Residual: colour-block `li` placement (all 8 blocks) and the reset block's store grouping (retail sched groups the constant-index stores by colour value). Tried: `-fno-schedule-insns{,2}` (1119/1098), `-O1` (939), cc1 2.8.1 (552).

## Round 3 (lane exeA3, 2026-09-27) — 0 mismatches pending one maspsx extension
Draft build/lanes/exeA3/drafts/func_8002BC90.c links word-exact (era_link_check + al.sh aligned all words) with the
current env plus `MASPSX_HOIST_BLOCK_LI=D_8009CDDC` (K4, landed) and `MASPSX_HOIST_BLOCK_LI_TAIL1=D_800B0130`
(prototype in build/lanes/exeA3/maspsx via ERA_LINK_CHECK_MASPSX; spec build/lanes/exeA3/KNOB_REQUESTS.md): the
D_800B0130 gauge blocks keep n-1 constants before reload 1 and the last one after reload 1. Other levers: per-block
`register int k asm("$N") = K;` colour-constant pins; GCC 2.7.2 in-struct alias choices (sym[1] asm-label aliases,
per-site struct-member derefs) to reproduce retail's load/store scheduling. See build/lanes/exeA3/results.jsonl.
