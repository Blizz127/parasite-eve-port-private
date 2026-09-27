# func_80042020 — memcard slot select (sprintf status line, arm D_800A1854 op)

- VRAM `0x80042020`, size `0x150`, profile `era_o2_g8_force_d8009ee70_d80092224_d800a1854_d800a1858_absolute`. Source: `src/func_80042020.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80042020 0x150`.
- Prior: parked(lane2)

## Levers
1. -G8 + FORCE_ABSOLUTE with D_8009EE70 declared as a scalar char (cc1 sees a 1-insn gp constant so it rematerialises la per use instead of holding it in a callee-saved reg); count update as int m = c->count; c->count = (e->state != st) ? m + 1 : m; (ternary without the load local kept &D_800A0ED4 in s4); c->f1/c->f7 = 1 written before h14/h16 puts li 1 at the head of the block. Prior lane-2 park draft reused

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80042020
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80042020
disc1_preflight: PASS (deep, 1893 c / 277 asm / 2 rodata)
LEAF_STRONG=EXACT func_80042020 span=0x150 vram=0x80042020 compiled=0x150 failures=[none]
```
