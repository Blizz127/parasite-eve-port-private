# func_80073A44 — LEAF_STRONG=EXACT (lane exeD, 2026-09-27)

Resolves `PARK.md` in this directory. That park was done on cc1 2.7.2 only, and its
"prologue-schedule" residual is ordinary cc1 2.8.1 behaviour.

- VRAM `0x80073A44`, file `0x64244`, size `0x178` (94 words). On-path fan-in 17: the VSync read
  called from the boot tail, `func_8006A5BC` and `func_800811E4`.
- Profile `era_o2_g0_no_schedule_insns_cc1_281` (new):
  `-O2 -G0 -mno-split-addresses -fno-schedule-insns`, `ERA_CC1_VER=2.8.1`.
- YAML: `- [0x64244, c, func_80073A44]` inserted after `[0x64228, asm]`. The span ends at the existing
  `[0x643BC, c, func_80073BBC]`, so no resume row is needed.

## Verify

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD/disc1.yaml --profiles build/lanes/exeD/disc1_build_profiles.json --deep --only func_80073A44
  -> disc1_preflight: PASS (deep, 2044 c / 218 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD/disc1.yaml --profiles build/lanes/exeD/disc1_build_profiles.json func_80073A44
  -> LEAF_STRONG=EXACT func_80073A44 span=0x178 vram=0x80073A44 compiled=0x180 failures=[none]
```
`profile_necessity.py` skipped: no `--config` option.

## How each PARK residual closed

1. *Pointer loads hoisted above `addiu $sp`* — this is plain cc1 2.8.1 behaviour: 2.8.1
   schedules the RTL prologue together with the body.
2. *Loop temp spilled to the stack at 0x10* — the temp is a `volatile unsigned int t;` local, so the
   read loop is `lw / sw 0x10 / lw 0x10 / lw / bne`. The status and counter pointers are
   pointer-to-volatile (`extern volatile unsigned int *D_80094574, *D_80094578`).
3. `D_8009457C` and `D_800956AC` are `volatile` globals. The tail re-seed loop re-reads
   `D_8009457C` after storing it, and the `*D_80094574` / `D_800956AC` read order is kept.

## Ladder (era_link_check, word mismatches, cc1 2.8.1)

| form | mismatches |
|---|---|
| first draft (ternary args) | 92 (46 diff lines) |
| separate `if` blocks for the two call args; `v = D - 1; v += a0` | 30 diff lines |
| + volatile `D_8009457C` | 26 diff lines |
| + `register int v asm("$2")`, so `v` is not allocated to `$a0` and `a0` is not copied to `$a2` | 8 |
| + volatile `D_800956AC` | 1 (`addu a0,a0,a1`) |
| + `-fno-schedule-insns` | **0** |

The last word: with sched1 on, `li a1,1` is moved ahead of `addiu a0,a0,1` before reload.
Post-reload CSE then rewrites the `+1` as `addu a0,a0,a1`. An opaque-1 `asm` barrier or a
temp local does not close it; turning sched1 off does. `-fno-expensive-optimizations` gives 6;
cc1 2.8.0 builds are identical to 2.8.1 here.
