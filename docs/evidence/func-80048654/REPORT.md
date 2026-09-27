# func_80048654 — item list screen open

- VRAM `0x80048654`, size `0x1E4`, profile `era_o2_g8`. Source: `src/func_80048654.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80048654 0x1E4`.


## Levers
1. `-G8` for D_8009CF94/D_8009CF8C/D_8009CF00 (gp 0x224/0x21C/0x190).
2. Handler slots are data symbols of text functions (func_80048838, func_80044444, ...), emitted as HI16/LO16 pairs.
3. The two handler addresses reused by both lists are locals pinned `register void *h1 asm("$17") = func_80044444; register void *h2 asm("$16") = func_80058030;` (unpinned: 19 words, $s0/$s1 swapped); the third list then naturally lands in $s1 after h1 dies.
4. Statement order `h1 = ; h2 = ; w->f2C = h1; w->f40 = 1; l->f30 = ; l->f84 = h2;` (179 other orders of that block tested; 4 exact).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80048654
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80048654
disc1_preflight: PASS (deep, 1845 c / 296 asm / 2 rodata)
LEAF_STRONG=EXACT func_80048654 span=0x1E4 vram=0x80048654 compiled=0x1F0 failures=[none]
```
