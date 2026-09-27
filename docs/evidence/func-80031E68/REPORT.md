# func_80031E68 — target highlight sprites

- VRAM `0x80031E68`, size `0x1F4`, profile `era_o2_g0`. Source: `src/func_80031E68.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80031E68 0x1F4`.


## Levers
1. Entry = { DR_TPAGE (8 bytes); SPRT (20) } = 28 bytes, table `D_8009E3B8[side][3]`; func_80077AC4 is the addPrim-style link (ot+28, entry).
2. Row-plus-index pointer form `(&(D_8009E3B8[b] + i)->sp)->u0` (as in the matched neighbour func_80031D6C) makes cc1 keep `la` bases in $s1 (entry+8) and $s3 (entry); `T[b][i].sp.u0` folds the symbol into every store (120 words).
3. The count is a 2-bit bitfield at +0x10 of D_8009D278->f68 (bits 4..5): bitfield promotion to int gives the signed `slt` loop test; an explicit `(x >> 4) & 3` gives `sltu`.
4. `->v0 = 224` is written in BOTH arms of the if/else: jump.c cross-jumps the identical tails, which is why retail's join reuses the arm-computed `i*28` and hoists 224 into $s2 (single store after the if: 105-116 words).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80031E68
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80031E68
disc1_preflight: PASS (deep, 1846 c / 296 asm / 2 rodata)
LEAF_STRONG=EXACT func_80031E68 span=0x1F4 vram=0x80031E68 compiled=0x200 failures=[none]
```
