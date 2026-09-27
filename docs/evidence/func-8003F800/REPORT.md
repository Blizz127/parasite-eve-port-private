# func_8003F800 — Save-block serializer

- VRAM `0x8003F800`, size `0x3D8`, profile `era_o2_g0`. Source: `src/func_8003F800.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8003F800 0x3D8`.


## Levers
1. Mirror of func_8003FBD8: memcpy(D_800A0ED0, &field, n); D_800A0ED0 += n per field with gcc builtin memcpy block moves (runtime-alignment loops for 0x800/0x70, lwl/lwr/swl/swr for small fields); single bytes '*cur = x; cur++;'. First draft LINK_EXACT, plain era -O2 -G0.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8003F800
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8003F800
disc1_preflight: PASS (deep, 1883 c / 281 asm / 2 rodata)
LEAF_STRONG=EXACT func_8003F800 span=0x3D8 vram=0x8003F800 compiled=0x3E0 failures=[none]
```
