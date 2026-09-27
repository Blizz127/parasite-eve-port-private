# func_8005E12C — pad event pump

- VRAM `0x8005E12C`, size `0x1E0`, profile `era_o2_g8`. Source: `src/func_8005E12C.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8005E12C 0x1E0`.


## Levers
1. `-G8`: queue globals D_8009D0DC..D_8009D0F8 are gp-relative (0x36C..0x388).
2. The queue push is a `static inline` helper (free-list pop, append to D_8009D0E0/E4 with func_800527C0(31) on a corrupt empty queue) expanded twice; the type-4 constant lands in $s4 exactly as retail.
3. Enabled test written `if (D_8009D0E8 != 0) { ... } else if (btn == 0) D_8009D0E8 = 1;` so the disabled arm is laid out last (the early-return form: 91 words).
4. Repeat-rate select `(D_8009D0F8 >= -299) ? 1 : 8` (the `< -299 ? 8 : 1` polarity: 3 words).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8005E12C
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8005E12C
disc1_preflight: PASS (deep, 1842 c / 298 asm / 2 rodata)
LEAF_STRONG=EXACT func_8005E12C span=0x1E0 vram=0x8005E12C compiled=0x1E0 failures=[none]
```
