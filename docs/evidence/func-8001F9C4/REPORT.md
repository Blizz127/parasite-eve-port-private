# func_8001F9C4 — entity status timers (poison/slow/stop/confuse counters, HP regen, auto-cure items)

- VRAM `0x8001F9C4`, size `0x818`, profile `era_o2_g8_expand_div_force_d8009d278_d8009d254_d8009d2e8_d8009d228_d8009cddc_absolute`. Source: `src/func_8001F9C4.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8001F9C4 0x818`.
- Prior: asm

## Levers
1. unsigned int *fl = &st->f4C flag pointer; (short)(x -= y) <= 0 timer tests; goto loop for the auto-heal item cascade; h = (short)f1C in an int temp before / 5 (else shorten_binary_op narrows the quotient to short); per-arm duplicated cure calls cross-jump like retail

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8001F9C4
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8001F9C4
disc1_preflight: PASS (deep, 1905 c / 274 asm / 2 rodata)
LEAF_STRONG=EXACT func_8001F9C4 span=0x818 vram=0x8001F9C4 compiled=0x820 failures=[none]
```
