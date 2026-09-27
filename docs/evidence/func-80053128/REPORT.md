# func_80053128 — item in-use flag rebuild

- VRAM `0x80053128`, size `0x18C`, profile `era_o2_g0`. Source: `src/func_80053128.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80053128 0x18C`.
- Prior: parked at 3 words by lane3

## Levers
1. Item{u8 b[32]}[0x80] at D_800C0EAC; bound D_800923D8 + 0x10 (lane3 typing).
2. `i = 0;` as a separate statement before `for (q = D_800C1F80; i < 0x52; i++, q++)` gives retail's `addu a0,zero,zero` ahead of the `lui/addiu` pair (the for-init comma form schedules the lui first: 3 words).
3. That reorder alone swaps $a0/$a1 (7 words); `register short *q asm("$5")` restores retail's allocation.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80053128
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80053128
disc1_preflight: PASS (deep, 1838 c / 300 asm / 2 rodata)
LEAF_STRONG=EXACT func_80053128 span=0x18C vram=0x80053128 compiled=0x190 failures=[none]
```
