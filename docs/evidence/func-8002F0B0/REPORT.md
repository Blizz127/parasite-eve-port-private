# func_8002F0B0 — battle death step

- VRAM `0x8002F0B0`, size `0x250`, profile `era_o2_g8_force_d8009d20c_d8009d254_absolute`. Source: `src/func_8002F0B0.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8002F0B0 0x250`.


## Levers
1. State byte D_8009CE74 (gp 0x104), switch 0/1/2 (compare tree; the `1` constant lives in $s0 and is reused for the case-2 `!= 1` test).
2. Case 0 actor walk: `if (a->info == 0) { if (a->f18C == 0) continue; } else { ... }` then the shared func_8003C5D8(&a->sub, 30) / h250 |= 2 tail (else-first layout: 107 words).
3. After `a->f98 |= 0x1000` retail RE-LOADS the flag word before the 0x40000000 test and reuses that value for `|= 0x10`: `v = *(volatile unsigned int *)&a->f98; if ((v & 0x40000000) && a->info->bAF == 0) { a->f98 = v | 0x10; a->info = 0; }` (plain C volatile read, no asm; an empty register barrier instead gives a register copy: 2 words; a volatile field makes case 1 reload too: 56 words).
4. Case 1 `ok` flag is `unsigned char` (retail `andi v0,a0,0xff`).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8002F0B0
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8002F0B0
disc1_preflight: PASS (deep, 1856 c / 295 asm / 2 rodata)
LEAF_STRONG=EXACT func_8002F0B0 span=0x250 vram=0x8002F0B0 compiled=0x250 failures=[none]
```
