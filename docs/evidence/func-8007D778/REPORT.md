# func_8007D778 — EXACT (lane exeD6, 2026-09-28)

VRAM `0x8007D778`, file `0x6DF78`, size `0x280` (SPU transfer-mode dispatcher, `mode, ...` varargs).
Profile `era_o2_g0_cc1_281` (cc1 2.8.1, `-O2 -G0 -mno-split-addresses`, no maspsx knobs).
YAML: `- [0x6DF78, c, func_8007D778]` (replaces `- [0x6DF78, asm]`; next row `[0x6E1F8, c, func_8007D9F8]` unchanged).
Source: `build/lanes/exeD6/drafts/func_8007D778.final.c` (also `src/func_8007D778.c`).

## Verification (private config build/lanes/exeD6/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD6/disc1.yaml --profiles build/lanes/exeD6/disc1_build_profiles.json --deep --only func_8007D778
  disc1_preflight: PASS (deep, 2111 c / 171 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD6/disc1.yaml --profiles build/lanes/exeD6/disc1_build_profiles.json func_8007D778
  LEAF_STRONG=EXACT func_8007D778 span=0x280 vram=0x8007D778 compiled=0x280 failures=[none]
```
profile_necessity.py has no `--config` (the leaf is not `c` in the main yaml yet), so the check was done by hand:
default `-O2 -G0` (2.7.2) = 144 mismatches; 2.8.1 without `-mno-split-addresses` = 140; assigned profile = LINK_EXACT.

## Levers (case 3 only; cases 0–2 were already exact in exeD5's draft)
Ladder from exeD5's best (12 word mismatches):
1. **Asm-output constant for chcr's high half.** `chcr = 0x1000000; asm("" : "=r"(chcr) : "0"(chcr));` sits before the
   `$4`-pinned `y` block, and `chcr |= 0x201; if (D_8009B44C == 1) chcr = 0x1000200;` comes at the end. The asm output
   stops combine from merging the `lui` into the later `|= 0x201` (in the exeD5 trace, combine merged it and the re-split
   insn was pinned below the barrier). `lui $6,0x100` then heads the join block, and reorg copies it into the `j` slot,
   as in retail. 12 → 6.
2. **`$4` pins on the size and on the volatile reload.** `register unsigned int sz asm("$4") = ap[0]`, with the DMA
   pointer loaded into a local `p` first and `register unsigned int a asm("$4") = D_8009B450` after the arithmetic.
   This gives retail's size-in-`$4` and the `D_8009B450` reload into `$4` after the `sltu`. 6 → 4 (an unpinned `sz` gives 7).
3. **Separate named `hi = sz >> 6; lo = (sz & 0x3F) != 0; t = hi + lo;`.** This gives retail's `srl`-before-`andi` order
   and `addu $3,$3,$2`. 4 → 0. The single expression gives 4, `lo` declared first gives 2, and `t += …` / `if (sz & 0x3F) t++` give 2.
