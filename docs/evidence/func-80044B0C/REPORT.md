# func_80044B0C — item action-menu handler

- VRAM `0x80044B0C`, size `0x2C0`, profile `era_o2_g8`. Source: `src/func_80044B0C.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80044B0C 0x2C0`.


## Levers
1. `-G8` for D_8009CF10/CDA8/CF04/CF0C/CF08; D_80092234 is an absolute `int [][3]` action table (menu, row).
2. `switch` on the action (compare tree 0/1/2/3, the constant 1 in $s3); the return value `r` lives in $s0 and is set to 1 in the shared tails.
3. Shared tails as labels inside the switch: `done:` (r = 1; func_800525EC()) placed in case 1 (retail cases 2/3 jump into case 1's tail), `fail:` (r = 1; func_800526C4()).
4. Case 0 reads D_8009CF04 once into a local for both calls (retail keeps it in $s0 across func_8005332C).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80044B0C
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80044B0C
disc1_preflight: PASS (deep, 1862 c / 291 asm / 2 rodata)
LEAF_STRONG=EXACT func_80044B0C span=0x2C0 vram=0x80044B0C compiled=0x2C0 failures=[none]
```
