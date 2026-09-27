# func_8004C608 — menu help-text line by help id (61-entry jump table)

- VRAM `0x8004C608`, size `0x648`, profile `era_o2_g8_dispatch_8001104c_force_d800c0e20_d800c0e22_absolute`. Source: `src/func_8004C608.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8004C608 0x648`.
- Prior: asm

## Levers
1. 61-case switch via DISPATCH_FOLD=jtbl_8001104C; k = D_8009CF8C local shared by cases 1/51/52 so the goto cf path feeds it straight to func_8005415C; (ptr + sel)[0x15] operand order; block-local t = D_8009CF18 * 3 - K for cases 29/60 (keeps the shared subu + func_8005DD3C tail); case 50 as if/else q = b + 4 : (unsigned char *)(sel + (int)b); goto-loop for the 9-bit selection count

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8004C608
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8004C608
disc1_preflight: PASS (deep, 1903 c / 274 asm / 2 rodata)
LEAF_STRONG=EXACT func_8004C608 span=0x648 vram=0x8004C608 compiled=0x650 failures=[none]
```
