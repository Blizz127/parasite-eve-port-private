# func_80023E14 — Battle item-use effect switch

- VRAM `0x80023E14`, size `0x43C`, profile `era_o2_g0_dispatch_800107a4`. Source: `src/func_80023E14.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80023E14 0x43C`.


## Levers
1. (1) MASPSX_DISPATCH_FOLD=jtbl_800107A4 (cases 6-17); (2) the four 2-bit status fields in Rec.f4C as bitfields b0/b2/b4/b6 with 'if (b == 0) b = 3; else if (b != 3) b = 0;' toggles (cross-jumped stores) and b12 clear; (3) f8 += f28/4 and /2 give the signed-division fixups; (4) (unsigned char)eff at the func_8003335C call (recomputed andi from the raw s1, 4 words otherwise)

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80023E14
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80023E14
disc1_preflight: PASS (deep, 1887 c / 280 asm / 2 rodata)
LEAF_STRONG=EXACT func_80023E14 span=0x43C vram=0x80023E14 compiled=0x440 failures=[none]
```
