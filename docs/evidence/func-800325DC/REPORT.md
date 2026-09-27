# func_800325DC — ATB gauge draw

- VRAM `0x800325DC`, size `0x1FC`, profile `era_o2_g0`. Source: `src/func_800325DC.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x800325DC 0x1FC`.


## Levers
1. Same DR_TPAGE+SPRT entry typing and `(&(D_8009E768 + b)->sp)->x0` pointer form as func_80031E68.
2. Status loop: signed-char `i` against `(signed char)func_80021054()`, `D_800BE834[i].type` (8-byte records) ladder `== 1` / `== 2` (0xC0 -> subtract low nibble, 0x40 -> n--) / `== 393`; cc1 cross-jumps the shared `if (x == K) n--` tails into retail's layout.
3. `n = r + (f0C & 0x3FF)` placed before the two sprite stores (natural order: 42 words).
4. `unsigned short y` (int y: 12 words — the D_8009D278 load gets hoisted above the prologue).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_800325DC
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_800325DC
disc1_preflight: PASS (deep, 1847 c / 296 asm / 2 rodata)
LEAF_STRONG=EXACT func_800325DC span=0x1FC vram=0x800325DC compiled=0x200 failures=[none]
```
