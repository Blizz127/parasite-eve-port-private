# func_8005CCA4 — new-game inventory setup

- VRAM `0x8005CCA4`, size `0x37C`, profile `era_o2_g8`. Source: `src/func_8005CCA4.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8005CCA4 0x37C`.


## Levers
1. D_800C0E00 typed as ONE player struct: stats (+6/+8/+0xA/+0xC/+0x20/+0x22/+0x24/+0x28[7]/+0x40), carried list +0x48, item records `Item items[128]` (32 bytes) at +0xAC, stock list `short stock[82]` at +0x1180. With one symbol cc1 derives &list from &h06 (+66) and &stock from the item end (+212) exactly as retail.
2. Two `static inline` searches (weapon: b6 != 9, armor: b6 == 9; both need b0 != 0 and b5 & 0x10) remove the matched item id (index + 256) from the stock list.
3. The found id is stored back into the loop counter `i` (retail keeps it in $s0; a separate `id` local stays in $a0: 86 words).
4. `c = t->b7` read right after the h06/h08 chain, and a separate `k` for the <= 50 clamp (reusing `c` costs 58 words); the three D_800A1xxx shorts are unsized arrays (absolute).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8005CCA4
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8005CCA4
disc1_preflight: PASS (deep, 1866 c / 288 asm / 2 rodata)
LEAF_STRONG=EXACT func_8005CCA4 span=0x37C vram=0x8005CCA4 compiled=0x380 failures=[none]
```
