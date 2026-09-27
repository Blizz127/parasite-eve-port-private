# func_80027A08 — Petrify/fade status step

- VRAM `0x80027A08`, size `0x30C`, profile `era_o2_g8_force_d8009d278_d8009d20c_d8009d254_d8009d208_d8009d1d4_d8009d23c_absolute`. Source: `src/func_80027A08.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80027A08 0x30C`.


## Levers
1. (1) result r pinned asm($4) (retail returns through a0; unpinned int r lets cse reuse its 1 for the f5==1 compare); (2) the 0x2000 arm ends 'if (f5==0 || f10<=0) r = 0; else { D_8009CE68 = 0xFF; r = 0; }' so r is dead at that join and reorg can fill the bne slot with a0=0x5B; (3) v pinned asm($5) and the fade byte decremented through a pinned temp t asm($2) (lbu v0; addiu a1,v0,-8); (4) for (i = 0; D_800B8A90[i]; i++) D_800B8A90[i] = 0 with u8 i; (5) FORCE_ABSOLUTE for the six in-gp-range absolute globals while D_8009CE68 stays 0xF8(gp)

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80027A08
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80027A08
disc1_preflight: PASS (deep, 1881 c / 281 asm / 2 rodata)
LEAF_STRONG=EXACT func_80027A08 span=0x30C vram=0x80027A08 compiled=0x310 failures=[none]
```
