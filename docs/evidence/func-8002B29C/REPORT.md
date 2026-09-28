# func_8002B29C — EXACT (lane exeA5, 2026-09-27)

- VRAM `0x8002B29C`, file offset `0x1BA9C`, size `0x6B0` (ends at `0x1C14C`, next row `[0x1C14C, c, func_8002B94C]`)
- 6-state fade/HUD state machine over `jtbl_80010910` (PolyFT4 `D_800BE9F0[D_8009CDDC]`).
- Profile `era_o2_g8_func_8002B29C`: `-O2 -G8`, env `MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_DISPATCH_FOLD=jtbl_80010910 MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D254,D_8009D20C,D_8009CDDC`
- Source: `src/func_8002B29C.c` (copy in `build/lanes/exeA5/drafts/`)
- Yaml row: `- [0x1BA9C, c, func_8002B29C]` (replaces `- [0x1BA9C, asm]`; no resume row needed)

## Verification (private lane config build/lanes/exeA5/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA5/disc1.yaml --profiles build/lanes/exeA5/disc1_build_profiles.json --deep --only func_8002B29C
  disc1_preflight: PASS (deep, 2093 c / 185 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA5/disc1.yaml --profiles build/lanes/exeA5/disc1_build_profiles.json func_8002B29C
  LEAF_STRONG=EXACT func_8002B29C span=0x6B0 vram=0x8002B29C compiled=0x6B0 failures=[none]
tools/analysis/era_link_check.py src/func_8002B29C.c 0x8002B29C 0x6B0 -O2 -G8   (env above, subshell)
  linked .text 1712 bytes, target 0x6b0, word mismatches=0
```
profile_necessity.py has no `--config` (run after integration).

## Levers (on top of the exeA3/exeA4 407/428 draft)
All residuals were sched1 placement of `la D_800BE9F0` and constants:
1. **Base-pointer local** `PolyFT4 *b = D_800BE9F0; p = &b[D_8009CDDC];` instead of `&D_800BE9F0[D_8009CDDC]` —
   the `la` is emitted before the index load, as in retail (cases 2, 3-then, 3-else). 407 → 417.
2. Case 2: pinned `k78 asm("$4") = 0x78`, `k7a asm("$5") = 0x7A`, then a zero-code `asm volatile("")` so both `li`
   stay at the block top (cc1 otherwise fills a reload's load-delay slot with `li 122`); `b` pinned `asm("$3")`
   and assigned between the g0 and b0 stores → `la $3` lands just before the third `sb`. 417 → 426.
3. Case 3 then-arm: `b = D_800BE9F0; { int k = 0x64; asm volatile(""); h &= 0xFF; … p->x0 = k; p->x2 = k; }` —
   `li 100` before the index load, `andi` after it. 426 → 428, 0 word mismatches.
