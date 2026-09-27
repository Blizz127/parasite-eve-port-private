# func_8008E2DC — matching C leaf

- VRAM `0x8008E2DC`, ROM offset `0X7EADC`, span `0x20C`
- Source: `src/func_8008E2DC.c`; YAML row `[0X7EADC, c, func_8008E2DC]`
- Build profile: `era_o2_g0_dispatch_8001213c`

## Levers
goto layout in retail block order; int c with unsigned casts on note-range tests; k--/k&=3 in place; lo += hi<<8; dispatch fold needs MASPSX_THREE_WORD_SYMBOL_STORE=1 alongside MASPSX_DISPATCH_FOLD=jtbl_8001213C

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8008E2DC
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8008E2DC
  -> LEAF_STRONG=EXACT func_8008E2DC span=0x20C failures=[none]
```
89 -> 78 -> 9 -> 2 -> 0 (new private profile era_o2_g0_dispatch_8001213c = -O2 -G0 + DISPATCH_FOLD + THREE_WORD)
