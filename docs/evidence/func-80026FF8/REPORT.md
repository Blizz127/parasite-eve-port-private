# func_80026FF8 — battle target-highlight flash (pulse brightness, highlight single/all/arc targets)

- VRAM `0x80026FF8`, size `0x5D4`, profile `era_o2_g8_force_d8009d2b0_d8009d278_d8009d20c_d8009d254_absolute`. Source: `src/func_80026FF8.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80026FF8 0x5D4`.
- Prior: asm

## Levers
1. static inline hilite(e) (f5==1 -> owner, f5==4 -> every group member via D_8009D20C, else self) inlined at 4 sites; switch on (int)((f10 >> 6) & 3) gives retail's slti compare tree (unsigned gives a linear chain); D_8009CE6C signed char (li -8); arc test as a 3-way ladder c < -0x600 / c < 0x600 / else (a && range test is folded to one sltiu)

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80026FF8
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80026FF8
disc1_preflight: PASS (deep, 1902 c / 275 asm / 2 rodata)
LEAF_STRONG=EXACT func_80026FF8 span=0x5D4 vram=0x80026FF8 compiled=0x5E0 failures=[none]
```
