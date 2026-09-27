# func_8004ECB4 — equipment top-window handler

- VRAM `0x8004ECB4`, size `0x27C`, profile `era_o2_g8_dispatch_80011144`. Source: `src/func_8004ECB4.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8004ECB4 0x27C`.


## Levers
1. New profile `era_o2_g8_dispatch_80011144` (-O2 -G8, THREE_WORD_SYMBOL_STORE, DISPATCH_FOLD=jtbl_80011144; table cases 0..4).
2. `switch ((D_8009CF0C == 1 && sel == 2) ? 4 : sel)` gives retail's separate selector register.
3. Cases `break` to a single `return 1;` (per-case `return 1` makes v0 busy and moves the D_8009CF0C copy to $v1: 2 words).
4. Cases 2/3 share `show: func_80048918(id, -1, -1); func_800525EC();` via `id = 830; goto show;` (case 3 falls into it, as in retail's layout).
5. Window 59 is created into a FRESH variable `n` (reusing `w`, which is also the create argument, flips the `li a0,59` / `move s0,v0` order: 2 words).
6. gp stores: D_8009CFB8 (0x248), D_8009CF1C (0x1AC), D_8009CF30 (0x1C0); D_800A1888 is an unsized int[] (absolute).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8004ECB4
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8004ECB4
disc1_preflight: PASS (deep, 1858 c / 293 asm / 2 rodata)
LEAF_STRONG=EXACT func_8004ECB4 span=0x27C vram=0x8004ECB4 compiled=0x280 failures=[none]
```
