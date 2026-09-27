# func_8003C818 — Actor colour fade step

- VRAM `0x8003C818`, size `0x2D4`, profile `era_o2_g8_expand_div_force_d8009cddc_absolute`. Source: `src/func_8003C818.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8003C818 0x2D4`.


## Levers
1. (1) MASPSX_EXPAND_DIV=1 (the three unsigned-char / signed-char divides carry the break 7/6 guards; without it 103 words); (2) FORCE_ABSOLUTE D_8009CDDC while D_8009CDA0 stays 0x30(gp); (3) cr, cg, cb copies in natural order after the three step divides (other orders: 4-6 words)

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8003C818
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8003C818
disc1_preflight: PASS (deep, 1876 c / 282 asm / 2 rodata)
LEAF_STRONG=EXACT func_8003C818 span=0x2D4 vram=0x8003C818 compiled=0x2E0 failures=[none]
```
