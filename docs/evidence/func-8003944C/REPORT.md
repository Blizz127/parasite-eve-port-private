# func_8003944C — set font glyph by key

- VRAM `0x8003944C`, size `0x22C`, profile `era_o2_g0`. Source: `src/func_8003944C.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8003944C 0x22C`.


## Levers
1. Two inlined copies of one `static inline int find_pos(unsigned char cls)` (class search then slot search, ONE shared loop counter `i` — separate helpers give each loop its own register: 10 words); `cls` is evaluated before `sel = 0` / the data load exactly as retail.
2. Cursor/data via the D_80091A1C struct; the flag bytes D_80091A1D/1E/20 as separate byte globals (a struct view makes cse cache &f1D in $s0 across the func_800389DC call: 100+ words).
3. Range check spelled `key - 2 >= 69` (retail `addiu -2; slti 69`).
4. `int` result stored into the byte cursor, then `cur_attr()` reloads it (see func_80039898).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8003944C
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8003944C
disc1_preflight: PASS (deep, 1850 c / 296 asm / 2 rodata)
LEAF_STRONG=EXACT func_8003944C span=0x22C vram=0x8003944C compiled=0x230 failures=[none]
```
