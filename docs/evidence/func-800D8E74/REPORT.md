# func_800D8E74 — matching C leaf

- VRAM `0x800D8E74`, ROM offset `0XC9674`, span `0x408`
- Source: `src/func_800D8E74.c`; YAML row `[0XC9674, c, func_800D8E74]`
- Build profile: `era_o2_g0`

## Levers
Sibling of func_800D7FBC (same case-2 nested switch on a1[0]; case 0 discards the func_80077CF4 result; tail constants 0xE4 / w + 6 : 2 / 0x80). Case 1 branches on a1[7]: the else arm needs the quotient in a local, `v = D_800E27EC * 500 / 36 - 1000; a1[4] = D_800E2202 + v;`, so the D_800E2202 load schedules after the divide (inline forms: 9 words or worse). No real division here, so plain era_o2_g0.

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800D8E74
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800D8E74
  -> LEAF_STRONG=EXACT func_800D8E74 span=0x408 failures=[none]
```

