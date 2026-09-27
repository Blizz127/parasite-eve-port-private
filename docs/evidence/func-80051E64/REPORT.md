# func_80051E64 — Item stat decoder (clamp fields, isqrt, attribute switch)

- VRAM `0x80051E64`, size `0x328`, profile `era_o2_g8_dispatch_80011218`. Source: `src/func_80051E64.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80051E64 0x328`.
- Prior: blocked (tooling, hoisted la)

## Levers
1. (1) la-form MASPSX_DISPATCH_FOLD=jtbl_80011218 retargets cc1's hoisted 'la $6,$L' to the pool table; (2) block-scoped fresh locals per clamp (shared t: 18 words, global alloc shift); (3) static inline isqrt identical to func_8005186C; (4) word0/word4 bitfield struct, 1-bit flag assigns give lw/ori/sw

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80051E64
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80051E64
disc1_preflight: PASS (deep, 1869 c / 285 asm / 2 rodata)
LEAF_STRONG=EXACT func_80051E64 span=0x328 vram=0x80051E64 compiled=0x330 failures=[none]
```
