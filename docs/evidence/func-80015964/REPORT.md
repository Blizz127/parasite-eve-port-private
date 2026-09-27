# func_80015964 — task: set floor by entity roll (table lookup of floor id)

- VRAM `0x80015964`, size `0x154`, profile `era_o2_g0`. Source: `src/func_80015964.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80015964 0x154`.
- Prior: parked(lane1)

## Levers
1. row pointer local unsigned char *p = D_8009173C[row]; then D_8009180C[p[k]] keeps the la after the row*24 multiply (direct D_8009173C[row][k] lets sched hoist la above mfhi, 9 words). Prior lane-1 park draft reused

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80015964
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80015964
disc1_preflight: PASS (deep, 1895 c / 276 asm / 2 rodata)
LEAF_STRONG=EXACT func_80015964 span=0x154 vram=0x80015964 compiled=0x160 failures=[none]
```
