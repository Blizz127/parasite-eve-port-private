# func_8004324C — Battle command availability check

- VRAM `0x8004324C`, size `0x228`, profile `era_o2_g8`. Source: `src/func_8004324C.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8004324C 0x228`.


## Levers
1. (1) arm layout: if (id==5) {..} else switch(id) { default; case 0x13; case 6 } reproduces retail's 5/default/19/6 block order; (2) '&&' forms for the 5/6/0x13 arms but a ternary for the func_800579D4 arm (&& hoists s0=0 above the call); (3) grade decode identical to func_800512AC's item_grade; (4) ok = (v > 0) ? ok & 1 : 0 for the grade tail (the andi lands in the bgtz slot); (5) D_800C0E20/06/08 as unsized arrays stay absolute under -G8; gp flag D_8009CF3C = 0x1CC(gp)

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8004324C
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8004324C
disc1_preflight: PASS (deep, 1872 c / 285 asm / 2 rodata)
LEAF_STRONG=EXACT func_8004324C span=0x228 vram=0x8004324C compiled=0x230 failures=[none]
```
