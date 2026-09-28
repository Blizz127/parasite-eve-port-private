# func_8001220C (main) — EXACT (lane exeA6, 2026-09-28)

- VRAM `0x8001220C`, file offset `0x2A0C`, size `0x2EC` (187 words; ends at the existing `[0x2CF8, c, func_800124F8]`)
- YAML row: `- [0x2A0C, c, func_8001220C]` (replaces `- [0x2A0C, asm]`; no resume row needed)
- Profile: default `era_o2_g0` (era `-O2 -G0`, no maspsx knobs) — no assignment entry needed
- Source: `src/func_8001220C.c` (replaces the old Phase-5FM parked candidate; ex "accepted residual",
  `parked_blockers.json` `main-prologue-scheduling`)
- Caveat for the lead: the 9-word scratchpad stack handoff (sp → 0x1F8003FC around `jal func_8019234C`)
  is still the pre-existing fenced `__asm__ volatile` block (no C spelling); the only change to it is dropping
  its explicit `nop` (gas already fills the `jal` delay slot in reorder mode, so the old block emitted 2 nops).

## Verification (private config build/lanes/exeA6/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA6/disc1.yaml --profiles build/lanes/exeA6/disc1_build_profiles.json --deep --only func_8001220C
  disc1_preflight: PASS (deep, 2113 c / 170 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA6/disc1.yaml --profiles build/lanes/exeA6/disc1_build_profiles.json func_8001220C
  LEAF_STRONG=EXACT func_8001220C span=0x2EC vram=0x8001220C compiled=0x2F0 failures=[none]
tools/analysis/era_link_check.py src/func_8001220C.c 0x8001220C 0x2EC -O2 -G0
  linked .text 752 bytes, target 0x2ec, word mismatches=0, nonzero_pad=0
```

## What the old PARK got wrong / load-bearing levers (aligned ladder from 119/187)
The "prologue save-batching / register skew" was not a cc1-vs-ccpsx scheduler skew: retail's `$16/$18/$19/$20`
are **loop.c-hoisted invariants** set in the outer-loop preheader after the two init calls.
1. The A8-code three-way dispatch is a `switch (v)` (retail's binary decision tree `beq v,$19` / `sltu` / …;
   case bodies in order `0xA8000048`, `0xAA108448` (else-arm first: `if (!(fF5 & 2))`), `0xA9400048`, default). 119 → 157.
2. No `state_val` / `bitmask` / `data` / `flagbyte` variables: constants inline and `D_800B0CD8` accessed
   directly as a struct (`flags` at 0, `fF5` at 0xF5). loop.c hoists `0xA9400048`, `0x100000`, `&D_800B0CD8`
   and `&D_800B0CD8+245`, and cse's related-value logic reproduces retail's `lw -245($18)` in the volume gate. 157 → 169.
3. `dispatch = 0;` between the two init calls (retail `addu $17,$0,$0` in the second `jal` slot); test
   expressions without the `v` temp for `flags & 0x100000` and `D_800A7918 < 600`. 169 → 186.
4. `while (func_800698D4()) func_80073A44(0);` polarity; `flags &= ~0x100` (retail `-257`, was `~0x101`).
5. Skip chain as `if (D_8009D280 == K) goto chk;` ×7 with `chk:` on the `flags & 0x100` test (retail branches
   to that test, not the loop top; a `do … while` form puts the test after LOOP_CONT and loses the hoisted base). → 187/187, 0 mismatches.
