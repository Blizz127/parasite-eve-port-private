# func_8003E0A4 — 1/3 selector + two short stores

- VRAM `0x8003E0A4`, size `0x2C`, profile `era_o2_g0`. Source: `src/func_8003E0A4.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8003E0A4 0x2C`.
- Prior: parked at 3 words by an earlier lane (unpinned result register)

## Levers
1. block-local `register int v asm("$2") = 2` used as both the compare constant and the result (`if (x == v) v = 3; else v = 1;`) reproduces retail reusing $v0; unpinned cc1 picks $a3 (3 words) or swaps $v0/$v1 (6 words).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8003E0A4
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8003E0A4
disc1_preflight: PASS (deep, 1836 c / 302 asm / 2 rodata)
LEAF_STRONG=EXACT func_8003E0A4 span=0x2C vram=0x8003E0A4 compiled=0x30 failures=[none]
```
