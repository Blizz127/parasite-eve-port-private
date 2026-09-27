# func_800622BC — matching C leaf

- VRAM `0x800622BC`, ROM offset `0X52ABC`, span `0x2AC`
- Source: `src/func_800622BC.c`; YAML row `[0X52ABC, c, func_800622BC]`
- Build profile: `era_o2_g8_force_d800c0e44_absolute`

## Levers
Window-frame emitter: optional colour blend of D_8009D130 into D_8009D14C/D_8009D150 (per-channel pair averages clamped to 0xFF), four corner points pushed into the D_800A22B0[12] point ring via D_8009D148 (capacity check, x then post-increment y store), func_80061878(D_800930A8, a2), then the func_80062090 fade tile at (x-2, y-2, w+4, h+4) and a second blend from D_800C0E44. -G8 with D_800C0E44 forced absolute (existing profile; plain -G8 fails the link with GPREL16 truncation). Levers: signed colour word (sra), `unsigned int *p_ = D_8009D150` pointer local taken first (retail materialises the la early), point values read into int locals before the capacity check, and the two blends differ in one temp: the first reuses the blue channel temp for the blue+red average, the second uses a fresh temp (swapping either costs 14-15 words).

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800622BC
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800622BC
  -> LEAF_STRONG=EXACT func_800622BC span=0x2AC failures=[none]
```

