# func_80045A98 — equip-slot panel

- VRAM `0x80045A98`, size `0x274`, profile `era_o2_g8`. Source: `src/func_80045A98.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80045A98 0x274`.


## Levers
1. `-G8` for D_8009CF30/CF1C/CF18 (gp 0x1C0/0x1AC/0x1A8); D_800C0E20/E22 as signed-char arrays (absolute `lb`).
2. Three-arm preview selection each ending in `func_80059EC8(0, x)` (cc1 cross-jumps the shared call like retail); the `D_8009CF1C` test is threaded into the next statement exactly as retail.
3. Stat rows via Rec bytes +7/+8/+9 and shorts +0xE/+0x10/+0x12. Matched on the first draft.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80045A98
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80045A98
disc1_preflight: PASS (deep, 1861 c / 292 asm / 2 rodata)
LEAF_STRONG=EXACT func_80045A98 span=0x274 vram=0x80045A98 compiled=0x280 failures=[none]
```
