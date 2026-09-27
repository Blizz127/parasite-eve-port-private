# func_80022D7C — Status-effect particle spawner

- VRAM `0x80022D7C`, size `0x28C`, profile `era_o2_g8_force_d8009d278_absolute`. Source: `src/func_80022D7C.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80022D7C 0x28C`.


## Levers
1. (1) plain if/else-if chain; cc1 cross-jumps the shared func_8006DE80(code,0,x,y,z) tails exactly as retail; (2) one pointer local st = (a->f0 + 0xCC) (retail keeps it in a1); (3) D_8009D278 FORCE_ABSOLUTE while D_8009D208 stays 0x498(gp). First draft LINK_EXACT.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80022D7C
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80022D7C
disc1_preflight: PASS (deep, 1874 c / 284 asm / 2 rodata)
LEAF_STRONG=EXACT func_80022D7C span=0x28C vram=0x80022D7C compiled=0x290 failures=[none]
```
