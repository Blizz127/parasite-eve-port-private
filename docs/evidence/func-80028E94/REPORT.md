# func_80028E94 — battle entity death/fade state machine (fAC 0..3), drop-table fill, all-dead check

- VRAM `0x80028E94`, size `0x4F4`, profile `era_o2_g0`. Source: `src/func_80028E94.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80028E94 0x4F4`.
- Prior: asm

## Levers
1. plain -O2 -G0; switch on u8 state with case 0 falling through to case 1; unsigned char loop index for the 10-slot drop table; first draft matched

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80028E94
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80028E94
disc1_preflight: PASS (deep, 1900 c / 275 asm / 2 rodata)
LEAF_STRONG=EXACT func_80028E94 span=0x4F4 vram=0x80028E94 compiled=0x500 failures=[none]
```
