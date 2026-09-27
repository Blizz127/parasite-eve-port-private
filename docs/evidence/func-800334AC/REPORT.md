# func_800334AC — battle status-icon row (D_8009D278->f4C 2-bit/1-bit flags to icon sprites)

- VRAM `0x800334AC`, size `0x4F4`, profile `era_o2_g8_force_d8009d278_d8009d254_d8009cddc_d8009cdd8_absolute`. Source: `src/func_800334AC.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x800334AC 0x4F4`.
- Prior: asm

## Levers
1. D_8009D278/D_8009CDDC absolute (FORCE_ABSOLUTE under -G8); block-1 index as k=(f&0xC)^0xC; k=k==0; k<<=2 keeps the a1 home; short k keeps the ==3 compare constant separate from k=3 (int k CSEs them, 5-word residual)

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_800334AC
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_800334AC
disc1_preflight: PASS (deep, 1890 c / 278 asm / 2 rodata)
LEAF_STRONG=EXACT func_800334AC span=0x4F4 vram=0x800334AC compiled=0x500 failures=[none]
```
