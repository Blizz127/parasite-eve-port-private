# func_8001A918 — scene-header relocation

- VRAM `0x8001A918`, size `0xE0`, profile `era_o2_g8_force_d800b1620_absolute`. Source: `src/func_8001A918.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8001A918 0xE0`.
- Prior: parked by lane1 (1 extra word / $a0 temp)

## Levers
1. New profile `era_o2_g8_force_d800b1620_absolute` (-O2 -G8, MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800B1620): the header pointer is absolute (lui/lw) while D_8009D1FC/D_8009CE08/D_8009D1D8/D_8009CE14 stay gp-relative.
2. `register Hdr *tmp asm("$2") = D_800B1620; h = tmp;` reproduces the load into $v0 + `move a0`.
3. `register Hdr *h asm("$4")` and `register unsigned int f asm("$3") = h->f18` (reused in `h->f18 = h + f`) put the `lui 0x8000` before the gp store of h (2 words otherwise).
4. Early exit: block-local `register unsigned int *q asm("$2") = h->f28` (4 words otherwise: cc1 reuses $a0 once h is dead).
5. Loop temp `register unsigned int t asm("$4")` (lane1 draft).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8001A918
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8001A918
disc1_preflight: PASS (deep, 1839 c / 299 asm / 2 rodata)
LEAF_STRONG=EXACT func_8001A918 span=0xE0 vram=0x8001A918 compiled=0xE0 failures=[none]
```
