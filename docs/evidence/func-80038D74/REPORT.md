# func_80038D74 — font-page shuffle (CD read + seeded rand table)

- VRAM `0x80038D74`, file offset `0x29574`, size `0x410` (ends on the single `jr $ra`).
- Profile `era_o2_g0_expand_div_fill_branch_split_li` = `-O2 -G0` +
  `MASPSX_EXPAND_DIV=1` + `MASPSX_FILL_BRANCH_SPLIT_LI=1` (maspsx patch 19).
- Lane exeB, 2026-09-27. Draft base: `build/archive/fanout/decompA/drafts/func_80038D74.c`
  (previously PARKED at a 5-6 word sched1 residual).

## Verification (private lane config)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeB/disc1.yaml --profiles build/lanes/exeB/disc1_build_profiles.json --deep --only func_80038D74
  -> disc1_preflight: PASS (deep, 2042 c / 217 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeB/disc1.yaml --profiles build/lanes/exeB/disc1_build_profiles.json func_80038D74
  -> LEAF_STRONG=EXACT func_80038D74 span=0x410 vram=0x80038D74 compiled=0x410 failures=[none]
(MASPSX_EXPAND_DIV=1 MASPSX_FILL_BRANCH_SPLIT_LI=1 tools/analysis/era_link_check.py src/func_80038D74.c 0x80038D74 0x410 -O2 -G0)
  -> word mismatches=0, nonzero_pad=0, LINK_EXACT
```
`profile_necessity.py` has no `--config` (reads the main yaml), so it rejects a
private-config span; necessity was checked by hand instead (below).

## Load-bearing levers (mismatch ladder)

| variant | mismatches |
|---|---|
| prior draft (inits `i, cnt, n` before loop) | 5 |
| + `do { func_80071A64(seed); } while (0);` | 3 |
| + init order `cnt = 0; n = 0; i = 0;` | **0** |
| reorder alone, no `do/while(0)` | 6 |
| exact source, no `MASPSX_FILL_BRANCH_SPLIT_LI` | 216 |
| exact source, no `MASPSX_EXPAND_DIV` | 133 |
| exact source, plain `-O2 -G0` | 197 |

1. **A loop note is a sched1 barrier.** Retail keeps the zero-inits (`$s6`, `$s5`,
   `$s4`) and the hoisted `&D_8009ECD8` (`$s7`) *after* `jal func_80071A64` (srand),
   with `$s6 = 0` reorg-stolen into the call's delay slot. cc1's sched1 moves pseudo
   sets that already cross calls above a call; gcc-2.7 `sched_analyze_insn` turns any
   `NOTE_INSN_LOOP_BEG/END` in mid-block into a full barrier (`reg_pending_sets_all`).
   Wrapping the srand call in `do { } while (0)` puts a `LOOP_END` note right after the
   call, reproducing the retail order while leaving reorg's delay-slot fill intact
   (an empty `asm volatile("")` barrier also stops the hoist but blocks the delay-slot
   fill: 212 mismatches).
2. **Unscheduled order = source order**: once the barrier holds, the inits come out in
   source order, so they must be written `cnt, n, i`.
3. Patch 19 (`lui $s0` half of the `li $s0,0x80010003` rand-modulus constant in the
   preceding `bnez` delay slot) and the inline ASPSX div expansion stay required.
