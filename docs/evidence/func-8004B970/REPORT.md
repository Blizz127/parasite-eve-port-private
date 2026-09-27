# func_8004B970 — EXP review panel step

- VRAM `0x8004B970`, size `0x210`, profile `era_o2_g8`. Source: `src/func_8004B970.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8004B970 0x210`.


## Levers
1. `-G8`: D_8009CFE8/CFEC/CFF0/CF78 are gp-relative (0x278/0x27C/0x280/0x208).
2. `inc = D_8009CFE8 < D_8009CFEC` kept as a local (slt straight into $s1).
3. `if (n < 98) rem = ...; else rem = 0;` — writing `rem = 0;` before the if lets reorg pull it into the earlier func_8005DBF8 jal slot (128 words); the else form gives retail's `move s2,zero` in the beqz slot.
4. Clamp `D > 999999 ? 999999 : D` for the first number.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8004B970
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8004B970
disc1_preflight: PASS (deep, 1841 c / 298 asm / 2 rodata)
LEAF_STRONG=EXACT func_8004B970 span=0x210 vram=0x8004B970 compiled=0x210 failures=[none]
```
