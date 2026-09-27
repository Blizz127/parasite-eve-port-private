# func_80016910 — task/script command dispatch (sparse switch over opcode *a[0])

- VRAM `0x80016910`, size `0x4E8`, profile `era_o2_g8_force_d8009d254_d8009d2f0_d8009d20c_d800bd028_d800bd025_absolute`. Source: `src/func_80016910.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80016910 0x4E8`.
- Prior: asm

## Levers
1. retail's compare tree pivots one node early => an extra empty case below 0x835 (its test is deleted by jump.c but it counts in balance_case_nodes); written as case 0x7D0: break; (the exact value is unrecoverable, any value < 0x835 is byte-identical). D_800BD028/D_800BD025/D_8009CDF8 as short[2]/u8[3]/u8[3] arrays so 2.7's in-struct alias rule keeps a[i] reloads after each store; (short)(*a[k] >> 16) gives lh 2; D_800B0CD8 unsized array + pointer locals (p for 0x835, q for 0x400000/0x8000000 which cross-jump); separate Ent *n for the func_80035038 result

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80016910
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80016910
disc1_preflight: PASS (deep, 1899 c / 275 asm / 2 rodata)
LEAF_STRONG=EXACT func_80016910 span=0x4E8 vram=0x80016910 compiled=0x4F0 failures=[none]
```
