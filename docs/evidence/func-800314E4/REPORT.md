# func_800314E4 — Battle status panel mode dispatch

- VRAM `0x800314E4`, size `0x27C`, profile `era_o2_g0_dispatch_80010e4c`. Source: `src/func_800314E4.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x800314E4 0x27C`.


## Levers
1. (1) MASPSX_DISPATCH_FOLD=jtbl_80010E4C; (2) mode!=8 arm first (retail places the mode-8 block after the switch; ==8 first: 148 words); (3) direct (&D_8009E730[k].sp)->x0 stores with no Sprt* local (a local swaps a2/a3: 7 words); (4) case 0 and case 4 share the 325DC/327D8(a3,30) tail via cc1 cross-jumping

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_800314E4
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_800314E4
disc1_preflight: PASS (deep, 1870 c / 286 asm / 2 rodata)
LEAF_STRONG=EXACT func_800314E4 span=0x27C vram=0x800314E4 compiled=0x280 failures=[none]
```
