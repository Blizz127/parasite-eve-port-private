# func_800D8978 — PARKED (lane exeD, 2026-09-27)

VRAM `0x800D8978`, file `0xc9178`, size `0x1f4`, `asm/disc1/C8B88.s`.

- Best: 6 words (4 diff lines) under 2.7.2 -O2 -G0 (era_o2_g0).
- Draft: `build/lanes/exeD/drafts/func_800D8978.c`.
- Residual / levers tried: Retail schedules `lhu 4(s0)` before the `D_800E21F8` load and `lhu 2(s0)` before the `D_800E21FC` load. Tried: moving the `t` read before the +8 store (same), a `u` local for +2 (8), volatile field reads (8-12), and flags -fno-schedule-insns/-fno-schedule-insns2 (62/38), -fschedule-insns2, -fno-strength-reduce, -fno-rerun-cse-after-loop (no change), -fno-expensive-optimizations (19).
- Measured with `tools/analysis/era_link_check.py` through `build/lanes/exeD/t.sh`; the diff-lines counts come from an aligned objdump diff.


## exeD round 2 (2026-09-27)

- Best: 6 words under 2.7.2 -O2 -G0.
- Draft: `build/lanes/exeD/drafts/func_800D8978.c` (if present).
- Brute-forced all 180 legal orders of the six case-1 field statements (t/u reads, +6/+0xA/+4/+2 stores): best 6 (5 orders tie). The load order is not a statement-order lever.

## exeD3 round (2026-09-27)
- Still 6 words (two lhu a1-field loads should come before the absolute global lhu). Tried: volatile a1 pointer plus volatile global casts (3 forms, 107–113), array-typed globals `D_800E21F8[]` and direct `a1[k]` indexing (alias-class change, no effect), and 2.8.1 (104).
