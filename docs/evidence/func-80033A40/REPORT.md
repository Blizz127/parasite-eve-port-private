# func_80033A40 — battle active-status HP/ATB bars (TILE/POLY_G4/sprite primitives) then HP digits + status icons

- VRAM `0x80033A40`, size `0x6C4`, profile `era_o2_g8_expand_div_force_d8009d278_d8009d254_d8009d1a0_d8009cddc_absolute`. Source: `src/func_80033A40.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80033A40 0x6C4`.
- Prior: asm

## Levers
1. POLY_G4 stride 36 (not F4); stores through (&X[v])->f so la+index stays in a register, reads direct X[v].f (symbol+reg); x1 + 0x38 - w written 0x38 - w + x1 to block fold reassociation; w2 statement before w1 fixes the sched/alloc of the two divisions; D_8009CE84 as unsigned short[2]

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80033A40
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80033A40
disc1_preflight: PASS (deep, 1892 c / 277 asm / 2 rodata)
LEAF_STRONG=EXACT func_80033A40 span=0x6C4 vram=0x80033A40 compiled=0x6D0 failures=[none]
```
