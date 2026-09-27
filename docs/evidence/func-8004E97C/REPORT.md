# func_8004E97C — item screen reopen

- VRAM `0x8004E97C`, size `0x250`, profile `era_o2_g8`. Source: `src/func_8004E97C.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8004E97C 0x250`.


## Levers
1. `-G8` for the menu globals D_8009CF98/CF9C/CF0C/CEF0/CEFC/CEF8/CF00/CF94/CF8C/D008 (gp); D_800C0E46 short[] absolute.
2. Whole body under `if (D_8009CF98 != 0) { ... } else { func_800512AC(9, 0); }` (early return: 132 words).
3. Category popcount `for (i = 8; i >= 0; i--) { n += m & 1; m >>= 1; }` over `D_8009CEF0 & 0x1F` / `& 0x1EF`.
4. Window 1 is created into a fresh variable `nw` (reusing `w` from the window-0 block flips `li a0,1` / `move s0,v0` after the create: the 4-word residual that parked this leaf earlier in the session). Same lever as func_8004ECB4.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8004E97C
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8004E97C
disc1_preflight: PASS (deep, 1859 c / 292 asm / 2 rodata)
LEAF_STRONG=EXACT func_8004E97C span=0x250 vram=0x8004E97C compiled=0x250 failures=[none]
```
