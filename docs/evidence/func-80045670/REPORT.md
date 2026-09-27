# func_80045670 — Equipment stat comparison panel

- VRAM `0x80045670`, size `0x428`, profile `era_o2_g8`. Source: `src/func_80045670.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80045670 0x428`.


## Levers
1. static inline clamp999 (if (x >= 1000) x = 999); straight transcription of the three stat rows for the old/new and new-only layouts; gp flags D_8009CF1C/D_8009CF18. First draft LINK_EXACT.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80045670
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80045670
disc1_preflight: PASS (deep, 1886 c / 279 asm / 2 rodata)
LEAF_STRONG=EXACT func_80045670 span=0x428 vram=0x80045670 compiled=0x430 failures=[none]
```
