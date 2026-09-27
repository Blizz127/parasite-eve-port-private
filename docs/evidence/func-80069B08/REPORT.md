# func_80069B08 — LINK_EXACT (lane exeC, 2026-09-27)

- VRAM `0x80069B08`, file offset `0x5A308`, size `0x5E0` (376 words), frame `0x58`; terminal `jr $ra` at
  `0x8006A0E0`, next function `func_8006A0E8` at `0x5A8E8`.
- yaml: insert `- [0x5A308, c, func_80069B08]` after `- [0x5A0D4, asm]` (that asm span now holds only
  `func_800698D4`, `0x5A0D4 + 0x234 = 0x5A308`) and before `- [0x5A8E8, c, func_8006A0E8]`.
- New profile `era_o2_g0_dispatch_80011388`: `-O2 -G0`, `MASPSX_THREE_WORD_SYMBOL_STORE=1`,
  `MASPSX_DISPATCH_FOLD=jtbl_80011388` (same shape as the existing `era_o2_g0_dispatch_*` family).
- Boot-spine disc-read / title-prep state machine (10-state `switch` on `s2`, `jtbl_80011388`).
- **`src/func_80069B08.c` is registered in `configs/USA/disc1_nonmatching_sources.json` as
  `in-progress-checkpoint`; that entry must be dropped when this row is integrated.** The old checkpoint body is
  kept at `build/lanes/exeC/drafts/func_80069B08.checkpoint_orig.c`.

## Verification (private lane config `build/lanes/exeC/`)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json --deep --only func_80069B08
  dispatch dedup: stripped 40-byte duplicate table from ...func_80069B08.c.o
  disc1_preflight: PASS (deep, 2047 c / 214 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json func_80069B08
  LEAF_STRONG=EXACT func_80069B08 span=0x5E0 vram=0x80069B08 compiled=0x5E0 failures=[none]
```
Necessity: without the fold (or with `THREE_WORD` alone) the two table-address words (`0x80069CF8`/`0x80069D00`)
point at the object's own table → 2 words; `DISPATCH_FOLD` only retargets when the three-word gate is on.

## Levers (ladder, from the in-tree checkpoint)

The PARK's "1504 vs 1488" claim no longer held: the checkpoint compiled to 1488 bytes (211 words) with the current
maspsx; the per-symbol `MASPSX_SYMBOL_AT_TEMP=jtbl_80011388` the PARK asked for exists now but is not needed.
1. case 3: `if (v1 == 3) goto case3_three;` with the `s1 = 0xB4; s2 = 6` block placed *after* `case3_one`
   (retail layout) → 198 (size back to 1504).
2. case 4: `if (v1 >= 0) goto case4_nonneg; if (v1 == -2) goto m2; goto merge; case4_nonneg: if (v1 == 0) …` → 158.
3. Ternary polarity `s4 == s0 ? 3 : 4` and `s4 == s0 ? 1 : 2` (retail `bne` with the else value in the slot) → 155.
4. `set_s6:` block nested inside the `D_800B0DCD & 2` test (retail places it before the `s2 = 9` block) → 149.
5. **Cross-jump stop**: an empty `asm("");` at the top of the case-9 `dec_s1:` tail keeps case 6–8 and case 9 as two
   separate `j merge; s1--` copies (retail) instead of one merged block → 14 (size exact).
6. Prologue: **unpinned parameter** — drop `register int s4 asm("$20")` and use `a0` directly, with the inits in
   source order `s2, s6, s7, s1, t20`: the incoming-arg copy `move $s4,$a0` lands right after `sw $s4` → 7 → 0
   (with the `done_loop` rect stores ordered `w, x, y, h`, found by greedy line-move search).
7. Dispatch as a real `switch (s2) { case 0: goto case0; … case 6: case 7: case 8: goto case678; case 9: … }`
   (the computed-goto checkpoint also matched `.text` but emitted a static label table in `.rdata`; the switch
   table is proven identical to `jtbl_80011388` and stripped by the build's dispatch dedup).
