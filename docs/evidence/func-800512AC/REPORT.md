# func_800512AC — battle item-command switch

- VRAM `0x800512AC`, size `0x24C`, profile `era_o2_g8_dispatch_80011170_force_d8009d254_absolute`. Source: `src/func_800512AC.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x800512AC 0x24C`.


## Levers
1. New profile `era_o2_g8_dispatch_80011170_force_d8009d254_absolute`: -O2 -G8, MASPSX_THREE_WORD_SYMBOL_STORE=1, MASPSX_DISPATCH_FOLD=jtbl_80011170 (13-entry table in the rodata pool: cases 0,1,2,3,5,8,9,10,11,12; 4/6/7 -> default), MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D254.
2. Actor select `e = (D_8009D254 != 0 && *D_8009D254 != 0) ? *D_8009D254 : 0;` gives retail's `sltu/negu/and` (a nested `x != 0 ? x : 0` is folded away by fold()).
3. Case 5: two bitfield stores into e->f68->f0C (10-bit low field from item +0xA; 2-bit grade at bit 20). The grade is a `static inline int item_grade(int *arg)` that re-reads `((Item *)*arg)->b6` per use (retail reloads `*arg` on the else path); the target is `Stat *s = e->f68; s->grade = item_grade(arg);` so the second f68 load precedes the grade computation.
4. Weapon/armor record copies are struct assignments (6 words / 2 words) into *D_8009D1E0.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_800512AC
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_800512AC
disc1_preflight: PASS (deep, 1855 c / 295 asm / 2 rodata)
LEAF_STRONG=EXACT func_800512AC span=0x24C vram=0x800512AC compiled=0x250 failures=[none]
```
