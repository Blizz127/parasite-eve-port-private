# func_8005C688 — Usable-item bitmap builder

- VRAM `0x8005C688`, size `0x464`, profile `era_o2_g8`. Source: `src/func_8005C688.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8005C688 0x464`.


## Levers
1. (1) main scan as a label/goto loop (no loop notes: test at top, 'j top' with k++ in the slot, and no extra 8-byte stack slot that for/while loops over a global bound get); (2) static inline rec() lookup (func_8005332C shape, early return 0 out of range); (3) the two slot searches as 'for (p = base; p < end && !cond(p); p++) {}' -- an empty-body for with the && exit test is what makes cc1 2.7.2 peel TWO iterations with folded absolute field addresses (while-&& peels one, for+break peels none); (4) n = -n when found, D_8009D068 = n != 0

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8005C688
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8005C688
disc1_preflight: PASS (deep, 1889 c / 279 asm / 2 rodata)
LEAF_STRONG=EXACT func_8005C688 span=0x464 vram=0x8005C688 compiled=0x470 failures=[none]
```
