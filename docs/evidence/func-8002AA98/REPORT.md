# func_8002AA98 — battle sequence state machine (fade/flash of actor + D_800B0B38 partner, restore status-bar prim colors)

- VRAM `0x8002AA98`, size `0x650`, profile `era_o2_g8_dispatch_800108f0_force_d8009d254_absolute`. Source: `src/func_8002AA98.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8002AA98 0x650`.
- Prior: asm

## Levers
1. 8-state switch via DISPATCH_FOLD=jtbl_800108F0; every exit as break (per-case return r reserved v0); shared clr/fade labels via goto; in-place v <<= 3; v = ~v / v += 0x7F; POLY_G4/sprite color stores in natural field order reproduce the retail sched grouping; r = 1 set in both arms of the D_8009D2A0 test; register Ent *a asm("$5") for the case-2 guard

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8002AA98
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8002AA98
disc1_preflight: PASS (deep, 1904 c / 274 asm / 2 rodata)
LEAF_STRONG=EXACT func_8002AA98 span=0x650 vram=0x8002AA98 compiled=0x650 failures=[none]
```
