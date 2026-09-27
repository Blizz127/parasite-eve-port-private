# func_80040B80 — Save-file image builder

- VRAM `0x80040B80`, size `0x400`, profile `era_o2_g0`. Source: `src/func_80040B80.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80040B80 0x400`.


## Levers
1. (1) builtin memcpy block moves (header fields, 0x80/0x100/0x12E4 blocks with runtime-alignment loops + tail word) and memcpy(cur, &x, 4) of the CRC word through a stack slot; (2) CRC-16 (poly 0x1021, init 0xFFFF, ~crc & 0xFFFF) over D_8009EED0 with u16 counters; (3) local p = D_8009EED0 before the loop (retail's hoisted la in a2; indexing the array directly: 48 words); plain era -O2 -G0

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80040B80
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80040B80
disc1_preflight: PASS (deep, 1884 c / 281 asm / 2 rodata)
LEAF_STRONG=EXACT func_80040B80 span=0x400 vram=0x80040B80 compiled=0x400 failures=[none]
```
