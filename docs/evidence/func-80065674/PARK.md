# func_80065674 — PARKED (lane exeC, 2026-09-27)

- VRAM `0x80065674`, file offset `0x55E74`, size `0x298` (166 words), `asm/disc1/55E74.s`.

## This lane (exeC)

- 85 -> 73. In-loop exit as 'return 0' (retail's v0=0 in the bgez slot, j past the shared move) and volatile-cast r->off / r->hdr.w loads (retail loads off then hdr) -> 73; residual: j=0 no longer fills the bne slot after the volatile loads (+1 word shift), tail loads pos.b/speed before hdr (volatile there: 92), q = l + idx*2 operand order. Draft build/lanes/exeC/drafts/func_80065674.c (needs MASPSX_EXPAND_DIV=1).

## Prior fan-out record (build/archive/fanout/*/results.jsonl)

- `decompB`: {"status": "parked", "profile": "era_o2_g0_expand_div", "levers": "animation step over 16-byte records (24.8 fixed-point frame, loop flag 0x20 wraps via %, else clamps), first-draft C in drafts/", "note": "~143 words on first draft: retail frame is 16 bytes with no stores (unused local), early-return layout differs, loop.c builds an r+4 giv. Not iterated further."}
- `decompB`: {"status": "parked", "profile": "era_o2_g0_expand_div", "best_mismatches": 85, "draft": "drafts/func_80065674.c", "ladder": "155 (first draft) -> 154 (no pad: frame 0x10 comes from expand_div, not a local array) -> 154 (Rec struct with byte/word unions so no &r->pos address giv) -> 85 (all returns via goto out: fixes block layout of the early exits)", "residual": "85w, mostly downstream of 3 sched/layout diffs: (1) the in-loop `return 0` is cross-jumped into the shared `move v0,zero` block (retail keeps its own v0=0 in the bgez slot and jumps past the shared move); (2) r->off/r->hdr load order before the inner clear loop; (3) the tail loads r->hdr before pos.b/speed (retail after, a3 = a1>>8). The records are walked as &recs[i] (retail `move a2,a0` biv copy) - that part matches."}

## Lane exeC2 (2026-09-27): still 73

- Non-volatile off/hdr loads (85): j=0 goes back into the `bne` slot, but cc1 loads hdr then
  off with no load-delay nop, one word shorter than retail's off, hdr, nop.
  Single-volatile combinations and reorderings: all 85. Barriers on the off value: 131, size
  changes.
- `q = l + idx*2` operand order: `(t1>>8)*2 + l`, `&l[...]`, int-cast and `<<1` spellings
  all canonicalise to the same `addu a0,t0,v0` (85).
- Tail hdr-load placement: asm barriers tying r to a1/a3 before the hdr read: 144-146.
