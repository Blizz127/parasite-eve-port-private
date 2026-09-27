# func_8004A0C8 — menu equip-slot select step (item stat panel)

- VRAM `0x8004A0C8`, size `0x4A8`, profile `era_o2_g8_force_d800c0e20_d800c0e22_absolute`. Source: `src/func_8004A0C8.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8004A0C8 0x4A8`.
- Prior: asm

## Levers
1. per-arm func_80059EC8 calls cross-jumped; STAT(f,k) macro written it->f + (D_8009CFC8 == k ? D_8009CFCC : 0) so fold distributes with it->f loaded once before the split, evaluated twice for the clamp-999; tail (it->f14 ? func_80063198 : func_800631AC)(func_80062A34(1, 0xB)) keeps the result in v0 until the jalr delay slot; Item layout shared with func_80045670

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8004A0C8
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8004A0C8
disc1_preflight: PASS (deep, 1897 c / 275 asm / 2 rodata)
LEAF_STRONG=EXACT func_8004A0C8 span=0x4A8 vram=0x8004A0C8 compiled=0x4B0 failures=[none]
```
