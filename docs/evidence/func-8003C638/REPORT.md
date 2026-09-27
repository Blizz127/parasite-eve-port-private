# func_8003C638 — render-object fade tick

- VRAM `0x8003C638`, size `0x1E0`, profile `era_o2_g8`. Source: `src/func_8003C638.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8003C638 0x1E0`.


## Levers
1. `-G8` for the gp colour word D_8009CDA0 (0x30($gp)); D_8009CDDC declared as an unsized `short[]` so its `lh` stays absolute.
2. Signed-char counter at +0x8C (`lb`, bgez/`== c8D - 1` tests), saved b90..b92 across func_8003CAEC.
3. Tail statement order is load-bearing: `b94 += b8E; b95 += b8F; b96 += b93; c8C--;` (the natural 94/8C/95/96 order costs 10 words; 22 other permutations 6-10).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8003C638
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8003C638
disc1_preflight: PASS (deep, 1840 c / 299 asm / 2 rodata)
LEAF_STRONG=EXACT func_8003C638 span=0x1E0 vram=0x8003C638 compiled=0x1E0 failures=[none]
```
