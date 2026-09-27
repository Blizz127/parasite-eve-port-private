# func_8003EB04 — pad input step (controller state, button remap, 32-bit repeat counters, cheat sequence, analog-to-dpad, edge masks)

- VRAM `0x8003EB04`, size `0x570`, profile `era_o2_g8_force_d8009d1a0_d8009d280_d800be9a0_d800be9a2_d800be9a6_d800be9a7_d800b0cd8_d800b0dbf_absolute`. Source: `src/func_8003EB04.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8003EB04 0x570`.
- Prior: asm

## Levers
1. pad-type test as t = D_800BE9A0 & 0xF000; if (t == 0x4000 || t == 0x7000) {...} else {zero}; separate f0 local for the early flag path; m = ~D_800BE9A2 & ~0x6000 read before the D_8009D238 copy and b = ~D_800BE9A2 re-read after (retail keeps two nor); chained masks as separate &= statements; edge masks written (D_8009D26C ^ D_8009D238) & X twice

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8003EB04
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8003EB04
disc1_preflight: PASS (deep, 1901 c / 276 asm / 2 rodata)
LEAF_STRONG=EXACT func_8003EB04 span=0x570 vram=0x8003EB04 compiled=0x570 failures=[none]
```
