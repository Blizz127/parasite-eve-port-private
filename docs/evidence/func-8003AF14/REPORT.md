# func_8003AF14 — render-object update

- VRAM `0x8003AF14`, size `0x230`, profile `era_o2_g0`. Source: `src/func_8003AF14.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8003AF14 0x230`.


## Levers
1. Same Obj layout as func_8003C638 (+0x97..+0x99 bytes, +0x9A short, +0x9C flag halfword, +0x9E/+0x9F bytes, +0xBA short).
2. `D_8009CDDC` is an int global passed to `short` prototype parameters: cc1 narrows the load to retail's `lh` (the `== 0` test keeps `lw`).
3. The draw call `func_8003C0B4(o, h9A, b97, b98, b99)` is written in each of the three flag arms; jump.c cross-jumps them into retail's shared tail. Matched on the first draft.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8003AF14
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8003AF14
disc1_preflight: PASS (deep, 1853 c / 296 asm / 2 rodata)
LEAF_STRONG=EXACT func_8003AF14 span=0x230 vram=0x8003AF14 compiled=0x230 failures=[none]
```
