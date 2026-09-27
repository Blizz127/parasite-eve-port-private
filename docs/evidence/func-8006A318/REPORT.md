# func_8006A318 — matching C leaf

- VRAM `0x8006A318`, ROM offset `0X5AB18`, span `0x2A4`
- Source: `src/func_8006A318.c`; YAML row `[0X5AB18, c, func_8006A318]`
- Build profile: `era_o2_g0`

## Levers
Ent table struct; block-local t for (lo+1)+step association; natural loops

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8006A318
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8006A318
  -> LEAF_STRONG=EXACT func_8006A318 span=0x2A4 failures=[none]
```
148 -> 6 -> 0 (first draft structure exact on 2.7.2)
