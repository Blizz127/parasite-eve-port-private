# func_80021F38 — Battle turn step

- VRAM `0x80021F38`, size `0x2D8`, profile `era_o2_g8_force_d8009d254_d8009d278_d8009d2fc_absolute`. Source: `src/func_80021F38.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80021F38 0x2D8`.


## Levers
1. (1) if (cur->fE != 0xC) {main} else if (fF == f1A) face(...) puts the 0xC block last like retail; (2) static inline face(a): d = a->f26A - D_8009D27C before func_80079FB4(d, func_80030534()), 3-way sector k, func_8001A680(D_8009D254, *(k + D_8009D278->f14)); (3) D_8009CE39/3B/3C as elements of a sized gp array D_8009CE38[8] (array = in-struct, so the byte store blocks the later D_8009D278->f68 load exactly as retail; scalars: 47 words); (4) switch on the u8 func_8002312C result {0: break; 1: ...}; (5) register pins cur asm($5), main-body a asm($4), inline a asm($4) (unpinned: 5-42 words)

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80021F38
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80021F38
disc1_preflight: PASS (deep, 1877 c / 281 asm / 2 rodata)
LEAF_STRONG=EXACT func_80021F38 span=0x2D8 vram=0x80021F38 compiled=0x2E0 failures=[none]
```
