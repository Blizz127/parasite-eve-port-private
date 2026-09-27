# func_8004E704 — equipment screen open

- VRAM `0x8004E704`, size `0x248`, profile `era_o2_g8`. Source: `src/func_8004E704.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8004E704 0x248`.


## Levers
1. `-G8` for D_8009CF0C/CF94/CF8C/CF00/CEFC (gp 0x19C/0x224/0x21C/0x190/0x18C).
2. Same window struct and handler-slot convention as func_80048654 (text functions referenced as data symbols -> HI16/LO16).
3. `first = l->f80 == 0` computed before the window-52 calls (retail `sltiu s0` in the jal slot). Matched on the first draft.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8004E704
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8004E704
disc1_preflight: PASS (deep, 1854 c / 296 asm / 2 rodata)
LEAF_STRONG=EXACT func_8004E704 span=0x248 vram=0x8004E704 compiled=0x250 failures=[none]
```
