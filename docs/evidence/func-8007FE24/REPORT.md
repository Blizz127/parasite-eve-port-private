# func_8007FE24 — libds vsync state machine (0x8007FE24, 0x2D0)

Status: **matched** (tooling2, 2026-09-24), `LINK_EXACT` + `LEAF_STRONG=EXACT`.

- YAML: `- [0x70624, c, func_8007FE24]` (replaces the `[0x70624, asm]` row; next row `[0x708F4, c, func_800800F4]`).
- Profile (new): `era_o2_g0_cc1_281_fill_call_delay_slot_load_delay_nop_before_label`
  = cc1 2.8.1 `-O2 -G0 -mno-split-addresses` + `MASPSX_FILL_CALL_DELAY_SLOT=1` (patch 8)
  + `MASPSX_LOAD_DELAY_NOP_BEFORE_LABEL=1` (patch 14).

## Ladder (era_link_check, 2.8.1 unless noted)
| step | words |
|---|---|
| structural draft (goto-shared retry) | 119 (2.7.2: 156) |
| timer via D_8009B598 + buffer pointer local + patch 8 | 109 |
| inline KICK in every case (cc1 cross-jumps the tails like retail) | 74 |
| cmd 16/17: reset arm first | 36 |
| callback pointer D_800A36A0 volatile (retail reloads it) | 1 |
| + patch 14 (nop before the cross-jump label, 0x80080038) | **0** |

Without patch 8: 128; without `-mno-split-addresses`: 169; on 2.7.2 the structural draft is 156.

## Verification (private config build/fanout/tooling2/)
```
python3 tools/build/disc1_preflight.py --config build/fanout/tooling2/disc1.yaml --profiles build/fanout/tooling2/profiles.json --deep --only func_8007FE24
  -> disc1_preflight: PASS (deep, 1836 c / 304 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/tooling2/disc1.yaml --profiles build/fanout/tooling2/profiles.json func_8007FE24
  -> LEAF_STRONG=EXACT func_8007FE24 span=0x2D0 vram=0x8007FE24 compiled=0x2D0 failures=[none]
```
