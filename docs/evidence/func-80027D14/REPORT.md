# func_80027D14 — battle enemy per-frame update (status bitfield counters, ATB gauge, damage popup, death/kill handling)

- VRAM `0x80027D14`, size `0x860`, profile `era_o2_g0`. Source: `src/func_80027D14.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80027D14 0x860`.
- Prior: asm

## Levers
1. Rec word 0 as a union of int view + bitfields (c1:3@1, cnt:5@5, c2:2@11, st:2@13, x:6@24) with a second Bits *b alias (retail keeps r and b in s1/s2); cnt handled as if (b->b.cnt >= 0x1E) ... else b->b.cnt++; goto loops for the two D_8009D20C walks that exit to the function tail (a for keeps the 8-byte loop slot) but a real for for the group-highlight walk (loop.c hoists D_8009D254); kill block placed before the survivor search; register Rec *r asm("$17") pin; fD2/fD4/fD6 store order

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80027D14
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80027D14
disc1_preflight: PASS (deep, 1906 c / 274 asm / 2 rodata)
LEAF_STRONG=EXACT func_80027D14 span=0x860 vram=0x80027D14 compiled=0x860 failures=[none]
```
