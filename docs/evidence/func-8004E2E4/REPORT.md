# func_8004E2E4 — Sub-menu input handler

- VRAM `0x8004E2E4`, size `0x420`, profile `era_o2_g8_force_d800b0cd8_absolute`. Source: `src/func_8004E2E4.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8004E2E4 0x420`.


## Levers
1. (1) static inline confirm(): scan bytes until 0xFF with ok &= (*p++ == 0xF), then func_800512AC/func_80052594 or func_800526C4; (2) switch(f24) with case 0x17 first; (3) f48-(k-3)/(k-5) through a temp d (fold would reassociate); (4) every arm ends 'func_80062CB8(t); func_8005267C(); goto out;' with one 'out: return 1' (cross-jumping then keeps the last arm's tail, as retail); (5) 0x800 arm uses its own Task *u (retail keeps it in a0)

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8004E2E4
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8004E2E4
disc1_preflight: PASS (deep, 1885 c / 280 asm / 2 rodata)
LEAF_STRONG=EXACT func_8004E2E4 span=0x420 vram=0x8004E2E4 compiled=0x420 failures=[none]
```
