# func_80039678 — font glyph cursor step

- VRAM `0x80039678`, size `0x220`, profile `era_o2_g0`. Source: `src/func_80039678.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80039678 0x220`.


## Levers
1. D_80091A1C is a small struct: cursor byte at +3 (0x80091A1F), glyph data pointer at +0xC (0x80091A28); cc1 addresses the cursor stores in the search loops off the `&data` register minus 9, exactly as retail.
2. `static inline unsigned char cur_attr(void) { unsigned char *d = D_80091A1C.data; return (d + (d + D_80091A1C.cur)[29])[4]; }` gives the data-before-cursor load order (direct spelling: 10 words).
3. Class byte: `c = cur_attr(); __asm__("" : "=r"(c) : "0"(c));` (empty register barrier, accepted) makes cc1 re-derive `andi v1,a1,0xff` per use like retail (6 words without; a `$5` pin instead truncates in place: 105 words).
4. Search loops use `(p + i)[101]` operand order and the `i = 0` / `i = p[0]` loop-exit assignments; `unsigned char v` gives the redundant `andi` after `lbu`.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80039678
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80039678
disc1_preflight: PASS (deep, 1848 c / 297 asm / 2 rodata)
LEAF_STRONG=EXACT func_80039678 span=0x220 vram=0x80039678 compiled=0x220 failures=[none]
```
