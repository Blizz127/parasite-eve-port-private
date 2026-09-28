# func_8001D340 — PARKED (lane exeA, 2026-09-27, round 2)

- VRAM `0x8001D340`, file offset `0xDB40`, size `0x2194`
- Best: 2056/2149 words align (difflib), size exact-ish (lnk 2128 words)
- Profile: -O2 -G8, MASPSX_EXPAND_DIV=1, MASPSX_THREE_WORD_SYMBOL_STORE=1, MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D278,D_8009D254,D_8009D20C,D_8009D1A0,D_8009D2E8,D_8009D1E8,D_8009CDDC,D_8009D234,D_8009D1D4,D_8009D200,D_8009D2FC,D_8009D28C,D_8009D1AC,D_8009D1CE,D_8009D244,D_800BCF88
- Best draft: `build/lanes/exeA/drafts/func_8001D340.c` (not in `src/`)
- Alignment metric: `build/lanes/exeA/al.sh <draft> 0x8001D340 0x2194 "<env>" -O2 -G8`. This dumps the linked `.text` via `ERA_LINK_CHECK_DUMP`, runs objdump on retail and link, and does a difflib opcode alignment with branch targets normalised.

## Status

Player battle tick (AT gauge regen, AT/PE HUD colour phases on `D_800B00E8`/`D_800B0130`/`D_800B6928`, contact damage over the `D_8009D20C` actor list, knockback slide, damage numbers, death). Full draft written from the retail control flow (the `pc_port` port served as a map only). Nearly all remaining diffs are the **colour-block constant materialisation** shared with `func_800299CC`/`func_8002BC90`: retail loads each block's colour constants (`li $t0..$a0`) at the top of the block (three store-groups ahead of first use); cc1 emits each `li` in the load-delay slot of the `lw D_8009CDDC` right before its first store. Register numbers already match, so only the placement differs. Other remaining: `func_8006DE80` argument/`D_8009D28C = 3` store interleave at the death tail, and one `D_8009CE34`/`*fl` ordering.

## Round 3 (lane exeA3, 2026-09-27) — 0 mismatches pending one maspsx extension
Draft build/lanes/exeA3/drafts/func_8001D340.c links word-exact (era_link_check + al.sh aligned all words) with the
current env plus `MASPSX_HOIST_BLOCK_LI=D_8009CDDC` (K4, landed) and `MASPSX_HOIST_BLOCK_LI_TAIL1=D_800B0130`
(prototype in build/lanes/exeA3/maspsx via ERA_LINK_CHECK_MASPSX; spec build/lanes/exeA3/KNOB_REQUESTS.md): the
D_800B0130 gauge blocks keep n-1 constants before reload 1 and the last one after reload 1. Other levers: per-block
`register int k asm("$N") = K;` colour-constant pins; GCC 2.7.2 in-struct alias choices (sym[1] asm-label aliases,
per-site struct-member derefs) to reproduce retail's load/store scheduling. See build/lanes/exeA3/results.jsonl.
