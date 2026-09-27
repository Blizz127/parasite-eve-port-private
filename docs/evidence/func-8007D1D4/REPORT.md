# func_8007D1D4 — libspu _spu_init (0x8007D1D4, 0x280)

Status: **matched** (tooling2, 2026-09-24), `LINK_EXACT` + `LEAF_STRONG=EXACT`.

- YAML: `- [0x6D9D4, c, func_8007D1D4]` then `- [0x6DC54, asm]` (inside the former
  `[0x6D9CC, asm]` row; the 8 leading bytes stay asm).
- Profile: new `era_o2_g0_spu_init_8007d1d4` = cc1 **2.7.2** `-O2 -G0 -fno-strength-reduce`
  + `MASPSX_FILL_CALL_DELAY_SLOT=1` (patch 8) + `MASPSX_FILL_EPILOGUE_DELAY_SLOT=1` (patch 3)
  + `MASPSX_LOAD_DELAY_NOP_BEFORE_LOOP_LABEL=1` (new patch 21).
- Although the leaf sits in the 0x80071A84–0x8008D610 band, 2.7.2 is its compiler:
  2.8.1 (best 62 words) hoists the epilogue restores above the trailing global stores
  and fills the `if (mode == 0)` slot from the fall-through; 2.7.2 does neither
  (band-compiler survey: `docs/ai_context/TOOLCHAIN_REBUILD.md`).

## Ladder (word mismatches, era_link_check)
| step | words |
|---|---|
| structural draft, 2.8.1 -mno-split-addresses | 96 |
| … best 2.8.1 tuning (volatile regs, pointer walk, patch 8) | 62 |
| same draft on 2.7.2 | 22 |
| + patch 3 | 20 |
| last spucnt store through a volatile cast | 15 |
| fm/noise/cdvol/exvol fields volatile (keeps `li a1,16` last → jal slot) | 1 |
| + patch 21 (nop before the voice-loop head) | **0** |

## Verification (private config build/fanout/tooling2/)
```
python3 tools/build/disc1_preflight.py --config build/fanout/tooling2/disc1.yaml --profiles build/fanout/tooling2/profiles.json --deep --only func_8007D1D4
  -> disc1_preflight: PASS (deep, 1835 c / 305 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/tooling2/disc1.yaml --profiles build/fanout/tooling2/profiles.json func_8007D1D4
  -> LEAF_STRONG=EXACT func_8007D1D4 span=0x280 vram=0x8007D1D4 compiled=0x280 failures=[none]
```
