# func_80039184 — font glyph code decrement

- VRAM `0x80039184`, size `0x168`, profile `era_o2_g0`. Source: `src/func_80039184.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80039184 0x168`.
- Prior: parked by lane2 (store-then-reload, memory clobber ruled out; not attempted further)

## Levers
1. Same TU helpers as func_8003944C; `unsigned char *k = &D_80091A1D` for the check/decrement (la base).
2. The class argument re-reads the global `D_80091A1D % 10 != 0` after the func_800389DC call (reading through `*k` keeps k live across the call in $s0: 27 words).
3. No memory-clobber barrier needed (int result into the byte cursor forces the reload).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80039184
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80039184
disc1_preflight: PASS (deep, 1852 c / 295 asm / 2 rodata)
LEAF_STRONG=EXACT func_80039184 span=0x168 vram=0x80039184 compiled=0x170 failures=[none]
```
