# func_80057094 — commit edited weapon/armor

- VRAM `0x80057094`, size `0x414`, profile `era_o2_g8`. Source: `src/func_80057094.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80057094 0x414`.


## Levers
1. `-G8` for D_8009D070/D074/D048/D050 (gp); record buffers D_800A1F94/D_800A1FB4 and the mod table D_800A1E44 are absolute 32-byte Item structs (2-aligned: the whole-record assignments become retail's lwl/lwr block moves).
2. Stat clamp `e->hA = (CAP(e->b9 + e->h12) < e->hA + A.hC) ? CAP(e->b9 + e->h12) : e->hA + A.hC;` with `#define CAP(a) ((a) >= 1000 ? 999 : (a))` — with `(a) < 1000 ? (a) : 999` fold turns the outer ternary into a MIN_EXPR and cc1 stops re-evaluating the arms (196 words).
3. Equipped-weapon resolver in the goto-done form (as in the func_80057654 draft) with a `w = x` copy; the item-record branch is `&D_800C0E00.items[x - 0x100]` so cc1 addresses it off the `&D_800C0E00.b20` register (-8052) as retail.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80057094
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80057094
disc1_preflight: PASS (deep, 1867 c / 287 asm / 2 rodata)
LEAF_STRONG=EXACT func_80057094 span=0x414 vram=0x80057094 compiled=0x420 failures=[none]
```
