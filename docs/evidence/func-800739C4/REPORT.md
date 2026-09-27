# func_800739C4 — matched (lane 4, session 2026-09-22)

- VRAM `0x800739C4`, size `0x64`, source `src/func_800739C4.c`.
- Profile: `era_o2_g0_fill_branch_macro_split` (-O2 -G0, env MASPSX_FILL_BRANCH_MACRO_SPLIT=1).

## Verification (private lane config)

```text
python3 tools/build/disc1_preflight.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json --deep --only func_800739C4
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json func_800739C4
disc1_preflight: PASS (deep, 1657 c / 341 asm / 2 rodata)
LEAF_STRONG=EXACT func_800739C4 span=0x64 vram=0x800739C4 compiled=0x70 failures=[none]
```

## Load-bearing levers

maspsx patch 10 (MASPSX_FILL_BRANCH_MACRO_SPLIT=1) puts the second word of the D_80094568 `$at` store macro into the `beq` delay slot; without it 2 words differ. Plain 2.7.2 (2.8.1 -mno-split-addresses leaves the same 2-word residual). `switch` spelling from the earlier draft. Span 0x64; the 3 trailing nops (0x64228..0x64233) stay a separate asm row (a 0x70 span trips deep-interior-return). Supersedes docs/evidence/func-800739C4/PARK.md.
