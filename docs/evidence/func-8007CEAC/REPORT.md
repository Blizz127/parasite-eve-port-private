# func_8007CEAC — EXACT (lane exeJ, 2026-09-28)

- VRAM `0x8007CEAC`, file offset `0x6D6AC`, size `0x1A8` (ends on its `jr $ra`; the next row `0x6D854` is the
  existing `c` span `func_8007D054`, so no resume row is needed).
- libdma-style channel kick. It polls CHCR busy with a timeout printf, sets or clears the DICR enable bit by `intr`,
  ORs the DPCR enable bit, writes MADR/BCR, polls `*D_8009B32C & 0x40`, writes CHCR and reads it back.
- Source: `src/func_8007CEAC.c`.
- Profile `era_o2_g0_cc1_281_fill_call_delay_slot_numeric_load_aspsx`:
  `ERA_CC1_VER=2.8.1`, flags `-O2 -G0 -mno-split-addresses`, `MASPSX_FILL_CALL_DELAY_SLOT=1`,
  `MASPSX_NUMERIC_LOAD_ASPSX=1` (sched1 is ON; the exeG–exeI drafts used `-fno-schedule-insns`).
- YAML: `- [0x6D6AC, asm]` → `- [0x6D6AC, c, func_8007CEAC]`.

## New maspsx knob K5, landed: `MASPSX_NUMERIC_LOAD_ASPSX=1`
It covers an indexed load from a numeric address outside int16 (`lw $d,528486536($b)`, which cc1 2.8.1
`-mno-split-addresses` emits for `*(volatile u32 *)(0x1F801088 + (ch << 4))`). If `$d != $b`, the output is
`lui $d,%hi(N) / addu $d,$d,$b / lw $d,%lo(N)($d)`. If `$d == $b`, it is `lui $at / addu $at,$at,$b / lw $d,%lo(N)($at)`.
Without the knob, GNU-as gives `$at` with `addu $at,$b,$at`.
- Code: `tools/era/maspsx/maspsx/__init__.py`, numeric-operand load arm. It is env-gated and runs only when the
  variable is `"1"`, so flag-off output is byte-identical by construction.
- Test: `tools/era/maspsx/tests/test_numeric_load_aspsx.py` (5 tests, including flag-off == unset). It is registered
  in `scripts/setup_era.sh` `MASPSX_TRACKED` and in the `.gitignore` negation list.
- Suite: `cd tools/era/maspsx && python3 -m unittest discover -s tests -t .` gives `Ran 546 tests ... OK` (551 OK after K-CDKJT landed too).
- Retail survey (whole-EXE objdump, `lui $d,0x1f8x / addu $d,$d,$b / l* $d,off($d)` and the `$at` variant):
  2 dest-temp sites and 1 `$at` site, all three in this function (0x8007CEE4, 0x8007CF08, 0x8007CF4C).

## Verify (private config build/lanes/exeJ/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeJ/disc1.yaml --profiles build/lanes/exeJ/disc1_build_profiles.json --deep --only func_8007CEAC
  -> disc1_preflight: PASS (deep, 2145 c / 146 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeJ/disc1.yaml --profiles build/lanes/exeJ/disc1_build_profiles.json func_8007CEAC
  -> LEAF_STRONG=EXACT func_8007CEAC span=0x1A8 vram=0x8007CEAC compiled=0x1B0 failures=[none]
```
`profile_necessity.py` has no `--config` option: it reads the main YAML and rejects the private manifest with
"stale build override". It was skipped as the brief allows. The manual necessity ladder below shows the profile is
load-bearing (`era_link_check.py src/func_8007CEAC.c 0x8007CEAC 0x1A8 ...`):

| env / flags | word mismatches |
|---|---|
| default era_o2_g0 (2.7.2) | 90 |
| 2.8.1 `-mno-split-addresses` only | 71 |
| 2.8.1 + K5 (no FILL_CALL) | 64 |
| 2.8.1 + FILL_CALL (no K5) | 7 |
| **full profile** | **0** |
| full profile + `-fno-schedule-insns` | 85 |

## Load-bearing source levers (the full profile is kept; mismatches when each one is undone)
1. **`v` is one function-level variable for both the DPCR shift amount and the DPCR value**
   (`v = ch*4+3 … bit <<= v; v = *q; v |= bit; *q = v;`). It dies twice, so it is a global pseudo. global-alloc then
   gives it `$6`, the first register free of o `$2`, bit `$3`, q `$4` and dma `$5`. This also puts `lui $6,1` in the
   head `beq` slot, because reorg now sees `$6` written at the target. With a separate `sh` local the count is 25.
   With `v = *q | bit` (load into a temp) it is also 25.
2. **`do { v = ch << 2; v += 3; } while (0);`**: the loop notes weight the `ch` reference ×2 (flow's `REG_N_REFS`).
   That gives `ch` 7 refs, so its allocno priority beats `intr` (6 refs: 12/98 < 2/16; 7 refs: 14/98 > 2/16), and
   ch=`$s0`, intr=`$s1`, as in retail. Without it the s0/s1 swap costs 10. Writing both statements into `v`
   (not `v = ch*4+3`) gives retail's `sll $6,$16,2 / addiu $6,$6,3` with no local temp (2 words).
3. **`bit = 1; bit <<= v;`** instead of `bit = 1 << v;`. The shift is in place on the constant's register, which
   changes sched1's order so `sllv` comes before the `0x1F801080` `lui/ori` pair, as in retail (3 words).
4. Trailing `t = *dma; __asm__ volatile("");` (exeI lever): the empty asm stops sched2 from putting the final `sw t`
   after the restores (10 words).
5. From exeH, kept: the arms reuse the poll counter `i` as the byte temp, and `t = *(unsigned int *)D_8009B348` sits
   inside both arms (cross-jumping merges them into retail's unfilled `lw/nop/lw/nop/sw`).

Prior PARK history: `docs/evidence/func-8007CEAC/PARK.md` (exeD3 → exeI, 71 → 15 aligned).
