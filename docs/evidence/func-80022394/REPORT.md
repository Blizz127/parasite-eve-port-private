# func_80022394 — battle phase advance (slot walk + pool-table facing/sound pick)

- VRAM `0x80022394`, size `0x9E4`, profile `era_o2_g8_rodata_fold_8001074c_80010760_force_d8009d2a0_d8009d1a0_d8009d278_d8009d254_d8009d20c_d800942e4_absolute`. Source: `src/func_80022394.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80022394 0x9E4`.
- Prior: first draft 478 words (size +7); struct macros 340 -> 21 -> 19 -> 10 -> 0

## Levers
1. Two initialized locals (unsigned char[20] = D_8001074C bytes, unsigned short[20] = D_80010760 halfwords) folded with MASPSX_RODATA_FOLD=D_8001074C,D_80010760; retail copies them with lwl/lwr.
2. Struct-member field macros (SW/SH) for pointer stores let sched hoist later global loads above them (MEM_IN_STRUCT); the one store that retail keeps ahead of the gp byte stores (k==6 block, 0x4C &= ~0x200000) uses a plain cast (WP) so it conflicts and stays in order.
3. The two 'advance by 7 / by 1' exits are written as duplicated per-arm blocks (D_8009D1D4 += n; range test; func_8001A680; |= 0x200000; return) - cc1 cross-jumps them into retail's shared tail with the add left per arm; a goto-shared tail puts v in the wrong register (10 words).
4. The 0x80000 block reads D_8009D1D4 inside the for-init (after the struct stores) so the lbu schedules first as in retail.
5. func_80024A3C returns signed char (sll 24 test); func_800518A8 takes the D_8009D278->0x68 pointer (retail keeps it in a0).
6. D_800942E4 stride 0xA0C, test (x==1||x==2) -> addiu -1/sltiu 2.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80022394
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80022394
disc1_preflight: PASS (deep, 1907 c / 274 asm / 2 rodata)
LEAF_STRONG=EXACT func_80022394 span=0x9E4 vram=0x80022394 compiled=0x9F0 failures=[none]
```
