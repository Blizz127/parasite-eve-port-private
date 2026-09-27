# func_800275CC — Battle target highlight

- VRAM `0x800275CC`, size `0x43C`, profile `era_o2_g8_force_d8009d2b0_d8009d278_d8009d20c_d8009d254_absolute`. Source: `src/func_800275CC.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x800275CC 0x43C`.


## Levers
1. (1) static inline mark(a): f5==1 -> a->f18C, f5==4 -> flag every other f5==4 actor and return, else a; then f250 |= 0x20; (2) switch index cast to int so cc1 builds retail's 1 / <2 / 0 / 2 / 3 compare tree; (3) arc loop body as a three-way if (y < -0x600 / y < 0x600 / else) re-reading list[i].f8 per test (a merged y range test lets fold build an unsigned range check: 149 words); (4) FORCE_ABSOLUTE for the four in-range absolute globals while D_8009CE6C stays gp

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_800275CC
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_800275CC
disc1_preflight: PASS (deep, 1888 c / 280 asm / 2 rodata)
LEAF_STRONG=EXACT func_800275CC span=0x43C vram=0x800275CC compiled=0x440 failures=[none]
```
