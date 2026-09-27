# func_80054F58 — EXACT (lane exeC3, 2026-09-27)

- VRAM `0x80054F58`, file offset `0x45758`, size `0x44C` (275 words; compiled object 0x450 = body + 1 gas pad nop).
- Profile `era_o2_g8` (`-O2 -G8`). Default `era_o2_g0` misses (254 word mismatches), so the assignment is load-bearing.
- YAML: `- [0x45758, c, func_80054F58]` (next row `[0x45BA4, c, func_800553A4]` unchanged).

## Verify (private lane config build/lanes/exeC3/)
- `python3 tools/build/disc1_preflight.py --config build/lanes/exeC3/disc1.yaml --profiles build/lanes/exeC3/disc1_build_profiles.json --deep --only func_80054F58` → `disc1_preflight: PASS (deep, ...)`
- `python3 tools/analysis/leaf_strong_check.py --config ... --profiles ... func_80054F58` → `LEAF_STRONG=EXACT func_80054F58 span=0x44C vram=0x80054F58 compiled=0x450 failures=[none]`
- `era_link_check.py src/func_80054F58.c 0x80054F58 0x44C -O2 -G8` → `word mismatches=0`, `LINK_EXACT`
- profile_necessity.py has no `--config` (reads the main yaml) — checked by hand: `-O2 -G0` → 254 mismatches.

## Levers
Built on the decompA/exeC draft (inline rec(), pinned r `$19`, pinned `a` `$4`, restructured tail returns).
New closing lever (exeC3): third search loop written as
`asm volatile(""); i = 0; if (i < q[0x14]) { register int c asm("$5"); c = a; for (; i < q[0x14]; i++) ... }`.
The explicit guard puts the pinned `c = a` copy (`move a1,a0`) into the loop preheader AFTER the
`blez`, so sched1/reorg place `lbu; nop; blez; move v1,zero` exactly as retail. Ladder: prior draft
(c initialised in a block before the `for`) 5 words (reorg filled the blez slot with `move $5,$4`
and `move $3,$0` scheduled before the lbu) → guarded-preheader copy 0. Removing the barrier
threads the blez to the `return 3` block (137).
