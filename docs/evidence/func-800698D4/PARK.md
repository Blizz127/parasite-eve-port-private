# func_800698D4 — PARKED (lane exeC, 2026-09-27)

- VRAM `0x800698D4`, file offset `0x5A0D4`, size `0x234` (141 words), `asm/disc1/5A0D4.s`.

## This lane (exeC)

- in-tree src (accepted-residual, 140/141 words; one missing nop in the search-#3/#4 beqz delay slots). New fact: retail's final read is `lb v0,D_800B0DCD` (signed), in-tree src has `unsigned char` -> `lbu`: `extern signed char D_800B0DCD;` fixes that word (draft build/lanes/exeC/drafts/func_800698D4_signedchar.c; still 560 bytes vs 564 because of the delay-slot fill). goto-structured searches (48-50, still 560 bytes), return-if forms: no change. src/ left untouched (registered).

## Prior fan-out record (build/archive/fanout/*/results.jsonl)

- `decompB`: {"status": "parked", "profile": "era_o2_g0 (2.8.1 same)", "best_mismatches": 50, "draft": "drafts/func_800698D4.c", "note": "new draft (OpenPeImage): static inline CD wait-ready loop x4, CdlFILE local, separate `if (r == 0) goto; if (r == -1) goto;` tests (the && form folds to (r+1)<2). Everything matches up to 0x80069A4C; residual is only that retail leaves the beqz/beq slots of the SECOND file-pair's two result tests unfilled (nop, li v0,-1 after), where cc1 fills them as it does (and retail does) for the first pair; the +2 words then shift the tail (count is inflated by the shift). Inline-helper spelling worse (127)."}

## Lane exeC2 (2026-09-27): still 1 word short (560 vs 564 bytes)

- Tail spellings with `extern signed char` (`== 0 ? -2 : 0`, if/return, r-temp, `-(x==0) & -2`):
  all 560 bytes, 51 (the one missing word shifts everything after it).
- Cross-jump theory tested and refuted: duplicated failure-path tails plus an asm-label alias
  (`D_800B0DCDw asm("D_800B0DCD")`) and `asm volatile("")` on the success-path store do get
  jump2 to cross-jump all three tails into one new label, but reorg still steals `li v0,-1`
  into the search-#3 `beqz` slot (560, 51). Volatile tails stop the cross-jump (688).
- gcc-2.x `resource.c` (a local agbcc copy) shows reorg treats every register as live at a
  target only when `find_basic_block` fails: the insns right after the previous BARRIER must
  all be CODE_LABELs, and one of them must be a flow block head. Retail's missing steal at #3/#4
  (and its kept `s0=1` steal in the #3 `beq`) does not fit that rule cleanly. Unresolved.

## Lane exeC5 (2026-09-27)

- src/func_800698D4.c updated to the `extern signed char` + `== 0` tail (lb/sltiu now exact);
  `aligned_diff_insns=3` = the two #3/#4 `beqz` slot nops + trailing pad. Still 560 vs 564 bytes.
- Mechanism: for retail to leave the slot empty, reorg must see `$v0` live at the tail label
  (`mark_target_live_regs`). Our flow dump (`cc1 -df -dd`) shows the tail block's live-in set
  is only {29,30} and every `$v0` set before it carries REG_DEAD, so fill_simple steals `li v0,-1`.
  Tried: separate result var for the 2nd half, `goto end` form (same 560), `||` form (folds to
  `(v1+1)<2`), if/return and `r` temp tails. No change. Still accepted-residual.
