# func_8004A6CC — bonus-point panel draw

- VRAM `0x8004A6CC`, size `0x2D4`, profile `era_o2_g8`. Source: `src/func_8004A6CC.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8004A6CC 0x2D4`.


## Levers
1. `-G8` for D_8009CFAC/CFD0/CFD8/CF18/CFDC.
2. Stat block at D_800A1A00: `unsigned char b[3]` at +7 and `short h[3]` at +0xE; per-case constant indices make cc1 take `&b[i]` into $s0 and address `h[i]` off it (+7/+8/+9) exactly as retail.
3. `switch (D_8009CFD0)` cases 0/1/2 written out in full; cc1 cross-jumps their common `func_8005FF28` call into retail's shared tail.
4. The glyph argument is a ternary `D_8009CF18 ? D_8009CFD0 + 124 : D_8009CFD0 + 127` (an if/else with two calls: 4 words).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8004A6CC
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8004A6CC
disc1_preflight: PASS (deep, 1863 c / 291 asm / 2 rodata)
LEAF_STRONG=EXACT func_8004A6CC span=0x2D4 vram=0x8004A6CC compiled=0x2E0 failures=[none]
```
