# func_80015DAC — EXACT (lane exeA5, 2026-09-27)

- VRAM `0x80015DAC`, file offset `0x65AC`, size `0xB64` (ends at `0x7110`, next row `[0x7110, c, func_80016910]`)
- Script-opcode switch (100..410) over `jtbl_800101B0`.
- Profile `era_o2_g8_func_80015DAC`: `-O2 -G8`, env `MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_DISPATCH_FOLD=jtbl_800101B0 MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2F0,D_8009D254,D_8009D20C,D_800B0E64,D_800B0CEA,D_800B0DBE,D_800B0DB8,D_800B0DB9,D_800B0DFC,D_800B0DCE`
- Source: `src/func_80015DAC.c` (copy in `build/lanes/exeA5/drafts/`)
- Yaml row: `- [0x65AC, c, func_80015DAC]` (replaces `- [0x65AC, asm]`; no resume row needed)

## Verification (private lane config build/lanes/exeA5/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA5/disc1.yaml --profiles build/lanes/exeA5/disc1_build_profiles.json --deep --only func_80015DAC
  disc1_preflight: PASS (deep, 2092 c / 186 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA5/disc1.yaml --profiles build/lanes/exeA5/disc1_build_profiles.json func_80015DAC
  LEAF_STRONG=EXACT func_80015DAC span=0xB64 vram=0x80015DAC compiled=0xB70 failures=[none]
tools/analysis/era_link_check.py src/func_80015DAC.c 0x80015DAC 0xB64 -O2 -G8   (env above, subshell)
  linked .text 2928 bytes, target 0xb64, word mismatches=0
```
profile_necessity.py has no `--config` and rejects the lane profiles file (func_80015DAC is not a C span in the main
yaml yet) — run it after integration. The env is obviously load-bearing (dispatch fold + force-absolute are needed to link).

## Levers (on top of the exeA4 712/729 draft)
Case 400 (loop over 12-byte records, match → store three globals, return 1):
1. `Rec12 *e = &q[i];` — loop strength-reduces a separate giv, with the `e` copy at the loop head (retail `addu $3,$7`).
2. Why retail's giv is `$7` and not q's `$4`: in the match block retail's local-alloc puts the `0xFFFFFF` mask in `$4`
   while the giv is still live, so global-alloc can't give the giv `$4` (dumps: `-dl -dg`, allocno 429 conflicts).
   Reproduced with block pins in the match block: `register unsigned int m asm("$4"); m = 0xFFFFFF;` and the
   b8/w4 temporaries `register unsigned char b asm("$2")` / `register unsigned int w asm("$2")`
   (`b = e->b8; D_800B0DB9 = b; w = e->w4 & m; D_800B0DFC = D_800B0E64 + w;`). `lui $4,0xff` then fills the beq slot.
Case 406: a zero-code `asm volatile("");` between the `*(short *)(p + 6) = …` store and `D_800B0CE9[0]++` stops sched
hoisting `la D_800B0CE9` into the `lw`→`sh` load-delay slot (labels / no-code goto block boundaries did not).

Ladder (aligned/729): 712 (exeA4) → pinned-e copy loop 712 (lnk 728) → keepalive asm(q) 717 → m-pin $4 718 →
store-reorder probe 721 → m/b/w pins 727 → + asm volatile barrier 729, 0 word mismatches.
