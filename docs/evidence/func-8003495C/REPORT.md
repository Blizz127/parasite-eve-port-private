# func_8003495C — battle message/state ticker (D_8009D1AC mode bits 8-9, low byte = countdown; language-indexed message table select)

- VRAM `0x8003495C`, size `0x484`, profile `era_o2_g8_force_d8009d1a8_d8009d1ac_d800bceac_d800bcea8_d800bceb4_absolute`. Source: `src/func_8003495C.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8003495C 0x484`.
- Prior: asm

## Levers
1. switch (mode) for the 1/2 dispatch; nested if (v != 0) { if (v == mode) ... } else ... for the D_800BCEAC choice; low byte of D_8009D1AC accessed as *(unsigned char *)&D_8009D1AC; register unsigned int t asm("$4") = D_8009D1AC & ~0x300 pin reproduces retail a0 home for the mode-advance store

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8003495C
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8003495C
disc1_preflight: PASS (deep, 1896 c / 275 asm / 2 rodata)
LEAF_STRONG=EXACT func_8003495C span=0x484 vram=0x8003495C compiled=0x490 failures=[none]
```
