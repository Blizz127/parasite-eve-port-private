> **RESOLVED 2026-09-27 (lane exeC2): EXACT — see REPORT.md.**

# func_80068710 — PARKED (lane exeC, 2026-09-27)

- VRAM `0x80068710`, file offset `0x58F10`, size `0x484` (289 words), `asm/disc1/58F10.s`.

## This lane (exeC)

- 35 words (unchanged; pure caller-saved coloring: retail best=t2, by/found=t3, more=t4; ours by/found=t2, more=t3, best=t4). New this lane: merge sel into best (284, cross-jump size change), int found/more (102), both (286), pins best=$10 / found=$11,more=$12 (278/164, size change), found/more init hoisted to the top in 4 combos (115-129), best=0 split into per-path inits (284-286), top-init order permutations (37), cc1 2.8.1 +/- -mno-split-addresses (155).

## Prior fan-out record (build/archive/fanout/*/results.jsonl)

- `decompB`: {"status": "parked", "best": 35, "profile": "era_o2_g8", "note": "Render_SortOt (grid cursor navigation, recursive wrap). Logic/layout exact on 2.7.2 -G8: separate sel result + duplicated tail in the !dir wrap branch (retail cross-jumps into the shared store tail), wrap-scan by separate from grid x, later scans use EX(it[i]) temps (no x local). Residual 35 = pure caller-saved naming: retail best/found/more = t2/t3/t4, ours t4/t2/t3. Tried 60 decl permutations (no effect), init placement (best/found/more), best=i on the direct hit (worse), per-loop counters (worse)."}
