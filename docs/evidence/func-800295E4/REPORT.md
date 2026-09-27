# func_800295E4

- VRAM `0x800295E4`, file offset `0x19DE4`, size `0x22C` (139 words)
- Profile: `era_o2_g8_force_d8009d254_plus51_battle_menu_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D254,D_800915E0,D_8009D1A0,D_8009D2E8 + 48 D_800B01xx byte symbols (new profile)
- Source: `src/func_800295E4.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_800295E4
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_800295E4
LEAF_STRONG=EXACT func_800295E4 span=0x22C vram=0x800295E4
```

## Load-bearing levers

Follows the func_80020DD0 precedent: each `D_800B01xx` byte is an individual forced-absolute `unsigned char` extern (`-G8` keeps `D_8009D2EC`/`D_8009D2A0` gp-relative). **Source order must be entry-by-entry** (four entries at 0x134/0x144/0x17C/0x18C with offsets +0,+1,+2,+8,+9,+A, then 0x158/0x168/0x1A0/0x1B0): all six constants are then live at once and cc1 materializes them up front, as retail does. Value-grouped order costs 35 words. `D_800B0CE6` is an unsized array (address kept in `$a1` for the `|= 2`).
