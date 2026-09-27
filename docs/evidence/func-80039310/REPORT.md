# func_80039310 — font glyph code increment

- VRAM `0x80039310`, size `0x13C`, profile `era_o2_g0`. Source: `src/func_80039310.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80039310 0x13C`.
- Prior: parked by lane2 (store-then-reload, memory clobber ruled out)

## Levers
1. Same TU helpers as func_8003944C (`find_pos` inline with one shared counter; int result into the byte cursor, reloaded by `cur_attr()` — no barrier).
2. `unsigned char *k = &D_80091A1D;` gives retail's `la a0` base for the load/increment/store (direct global: 78 words).
3. Statement order `(*k)++; D_80091A1E = D_8009EE22[*k]; D_80091A20 = 0; func_800389DC(D_80091A1E);` (clear before the table read: 5 words).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80039310
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80039310
disc1_preflight: PASS (deep, 1851 c / 295 asm / 2 rodata)
LEAF_STRONG=EXACT func_80039310 span=0x13C vram=0x80039310 compiled=0x140 failures=[none]
```
