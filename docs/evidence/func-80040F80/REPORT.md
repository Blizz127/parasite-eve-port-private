# func_80040F80 — memcard abort active operation (close fd, delete temp save file)

- VRAM `0x80040F80`, size `0x188`, profile `era_o2_g8_force_d8009ee70_d80092224_d800a185c_d800a1854_d800a1838_absolute`. Source: `src/func_80040F80.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80040F80 0x188`.
- Prior: parked(lane2)

## Levers
1. same lever as func_80042020: -G8 + FORCE_ABSOLUTE with D_8009EE70 declared scalar char so la is rematerialised per use; slot = c - D_800A0ED4 precomputed before the first sprintf; CardRec layout shared with func_80042020 (fd int at +0xC). Prior lane-2 park draft reused

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80040F80
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80040F80
disc1_preflight: PASS (deep, 1894 c / 277 asm / 2 rodata)
LEAF_STRONG=EXACT func_80040F80 span=0x188 vram=0x80040F80 compiled=0x190 failures=[none]
```
