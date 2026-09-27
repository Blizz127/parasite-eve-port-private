# func_800258CC — Battle target cursor

- VRAM `0x800258CC`, size `0x30C`, profile `era_o2_g8_dispatch_80010884_expand_div_force_d8009d278_d8009d1f4_d8009d2b0_absolute`. Source: `src/func_800258CC.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x800258CC 0x30C`.


## Levers
1. (1) MASPSX_DISPATCH_FOLD=jtbl_80010884 + EXPAND_DIV (the two % D_8009D2B0 cycles keep their break guards); (2) D_8009D2B0 scalar + FORCE_ABSOLUTE (an unsized array hoists its la into s1), D_800B0E08 unsized (la + reload); (3) func_800314E4 called through an int-parameter prototype; (4) the case 0-3 flag f is a narrow type (char/short) -> retail's explicit copy into a2 (int f: 56 words); (5) hp > f2 operand order

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_800258CC
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_800258CC
disc1_preflight: PASS (deep, 1880 c / 280 asm / 2 rodata)
LEAF_STRONG=EXACT func_800258CC span=0x30C vram=0x800258CC compiled=0x310 failures=[none]
```
