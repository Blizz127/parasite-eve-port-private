# func_80035038 — scene entity alloc (pop free list, link, init fields, model/anim setup)

- VRAM `0x80035038`, size `0x520`, profile `era_o2_g8_force_d800b0e64_absolute`. Source: `src/func_80035038.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80035038 0x520`.
- Prior: asm

## Levers
1. D_800B161C typed as member p of one object at D_800B0E98 (10x48 int table + pad + p at +0x784) and read through int **pp = &D_800B0E98.p so cse relates the loop table base (pp - 0x784) and keeps pp in s2; list insert order e->next = after->next; e->prev = after; after->next = e; (char *)D_800B0CE2 != 0 kept as la+beqz; buf[8] (the rotated for loop adds its own 8-byte slot); loop bound re-read through unsigned int *wp so loop.c hoists it and cse2 leaves the retail a2 = n copy

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80035038
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80035038
disc1_preflight: PASS (deep, 1898 c / 275 asm / 2 rodata)
LEAF_STRONG=EXACT func_80035038 span=0x520 vram=0x80035038 compiled=0x520 failures=[none]
```
