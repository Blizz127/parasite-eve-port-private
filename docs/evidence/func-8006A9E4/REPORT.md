# func_8006A9E4 — matching C leaf

- VRAM `0x8006A9E4`, ROM offset `0x5B1E4`, span `0x35C` (215 words)
- Source: `src/func_8006A9E4.c` (replaces the earlier tracked WIP file); YAML rows
  `[0x5B1E4, c, func_8006A9E4]` + `[0x5B540, asm]`
- Build profile: `era_o2_g0` (cc1 2.7.2 `-O2 -G0`)
- Function: scene data load. A chain of CD read / poll blocks, each retried with goto,
  plus block copies.

## Levers
- A goto-based retry for each read/poll block. A structured `while` loop lets loop.c
  hoist invariants and bias the counters, which retail doesn't do.
- The block copies go through char-aligned block structs, which gives retail's copy sequence.
- The last poll puts the assignment in the loop condition
  (`while ((s = poll() != 0) != 0)`) to reproduce retail's compare shape.
- Ladder: 206 → 3 → 0 word mismatches.

## Verification
```
python3 tools/analysis/era_link_check.py build/fanout/decompB/drafts/func_8006A9E4.c 0x8006A9E4 0x35C -O2 -G0
  -> LINK_EXACT, 0 word mismatches   (decompB)
python3 tools/analysis/leaf_strong_check.py func_8006A9E4
  -> LEAF_STRONG=EXACT   (team-lead, live config, batch 10)
```
Note: `docs/evidence/func-8006A9E4/PARK.md` (tracked, from an earlier session) is now stale.
