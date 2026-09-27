# func_8003FBD8 — Save-block deserializer

- VRAM `0x8003FBD8`, size `0x3D4`, profile `era_o2_g0`. Source: `src/func_8003FBD8.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8003FBD8 0x3D4`.


## Levers
1. (1) each field is memcpy(&dst, D_800A0ED0, n); D_800A0ED0 += n; with gcc's builtin memcpy: unknown alignment gives the lwl/lwr + swl/swr pairs and, for 0x800/0x70 bytes, mips expand_block_move's runtime (src|dst)&3 check with an aligned and an unaligned 16-byte loop; (2) the two trailing single bytes are 'dst = *cur; cur++;' (load before the increment keeps all three cursor stores; '*cur++' lets the +=2 store die: 116 words); (3) plain era -O2 -G0

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8003FBD8
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8003FBD8
disc1_preflight: PASS (deep, 1882 c / 281 asm / 2 rodata)
LEAF_STRONG=EXACT func_8003FBD8 span=0x3D4 vram=0x8003FBD8 compiled=0x3E0 failures=[none]
```
