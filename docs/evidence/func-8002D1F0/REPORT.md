# func_8002D1F0 — EXACT (lane exeB3, 2026-09-27)

- VRAM `0x8002D1F0`, file offset `0x1D9F0`, size `0xA68` (HP-critical palette flash, damage popups,
  actor-list status walk). Supersedes `PARK.md` in this directory.
- Profile `era_o2_g8_hoist_block_li_force_cddc_d250_d254_d20c`: `-O2 -G8`,
  `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009CDDC,D_8009D250,D_8009D254,D_8009D20C`,
  `MASPSX_HOIST_BLOCK_LI=D_8009CDDC` (maspsx local patch K4; **uncommitted in the working tree at
  the time of this match** — it must land with or before this leaf).
- yaml rows: `- [0x1D9F0, c, func_8002D1F0]`, `- [0x1E458, asm]` (after `- [0x1BA9C, asm]`).

## Verification (private lane config)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeB3/disc1.yaml --profiles build/lanes/exeB3/disc1_build_profiles.json --deep --only func_8002D1F0
  disc1_preflight: PASS (deep, 2080 c / 194 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeB3/disc1.yaml --profiles build/lanes/exeB3/disc1_build_profiles.json func_8002D1F0
  LEAF_STRONG=EXACT func_8002D1F0 span=0xA68 vram=0x8002D1F0 compiled=0xA70 failures=[none]
MASPSX_HOIST_BLOCK_LI=D_8009CDDC MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009CDDC,D_8009D250,D_8009D254,D_8009D20C \
  tools/analysis/era_link_check.py src/func_8002D1F0.c 0x8002D1F0 0xA68 -O2 -G8
  word mismatches=0 (LINK_EXACT)
```
`profile_necessity.py` has no `--config`, so it rejects a lane-only span; necessity checked by hand:
without K4 643 mismatches; without force-absolute 620; default `-O2 -G0` 596. `MASPSX_HOIST_BLOCK_LI=1`
is also exact; the symbol-scoped form is recorded.

## Levers (mismatch ladder)
1. exeB draft (asm-label aliases `D_8009CDDC_b/_c/_d` on the last index read of arms 1–3 to stop
   the cross-jump) + `buf[0x1A0]` for the 448-byte frame: 642.
2. **K4 `MASPSX_HOIST_BLOCK_LI`**: the colour constants move to retail's places (first n-2 before
   reload 1, the last two after reloads 1 and 2): 642 → 105. Arm 0 exact; arms 1–3 had permuted
   constant registers.
3. **Local-alloc tie-break by live length.** Constants are QI pseudos allocated by local-alloc,
   priority `floor_log2(refs)*refs/(death-birth)`, ties by qty number (birth order). From the `-dl`
   dump, arm 1/3 lengths were 80:63, 163:62, 190:61, so 163 outranked 80 (retail has 80 first). One
   `asm("");` after the `D_800B692C = 0x50` store (inside 163's range, after 80's death) makes 80 and
   163 tie, and 80 wins on qty order: 105 → 81. Arm 2: `asm("");` after `D_800B00FC = 0x9F` ties 159/255
   the same way: 81 → 75.
4. Block `0x5E`: store `+0x5C` before `B(+0x5F) = 1` (sched1 hoists the byte store across it, as in
   retail, while both halfword loads share one `D_8009D254` base): 75 → 3.
5. Actor walk: `{ unsigned char *t = W(s1,0); if (t == 0) continue; s0 = t; }` gives the load into
   `$2` with `addu $16,$2,$0` in the `beq` delay slot: 3 → 0 (`s0 = ...; if (s0 == 0)` and
   `if ((s0 = ...) == 0)` both leave 3).
