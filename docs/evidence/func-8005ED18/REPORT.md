# func_8005ED18 — POLY_FT4 glyph emit into the OT

- VRAM `0x8005ED18`, size `0x1B0`, profile `era_o2_g8`. Source: `src/func_8005ED18.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8005ED18 0x1B0`.
- Prior: parked at 2 words by lane3 (p pinned to $17)

## Levers
1. PSY-Q addPrim through a P_TAG bitfield (`Tag.addr:24`), as in func_8005EB64.
2. `register unsigned char *g asm("$16")` for the func_8005DADC rect pointer (unpinned cc1 swaps p/g in $s0/$s1: 78 words).
3. block-local `register PolyFT4 *q asm("$4") = p` for the geometry/UV stores reproduces retail addressing them off $a0 (an unpinned copy or a static-inline param copy puts the copy on g instead: 27 words; pinning p itself instead leaves the final addPrim `and` writing $s1: 2 words).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8005ED18
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8005ED18
disc1_preflight: PASS (deep, 1837 c / 301 asm / 2 rodata)
LEAF_STRONG=EXACT func_8005ED18 span=0x1B0 vram=0x8005ED18 compiled=0x1B0 failures=[none]
```
