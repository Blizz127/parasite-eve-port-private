# func_80081D74 — matching C leaf

- VRAM `0x80081D74`, ROM offset `0X72574`, span `0x84`
- Source: `src/func_80081D74.c`; YAML row `[0X72574, c, func_80081D74]`
- Build profile: `era_o2_g0_cc1_281`

## Levers
base pointer local p=&D_8009B70C; early return 0; explicit one=1 local; empty register barrier on one before the final store so return 1 stays li v0,1

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_80081D74
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_80081D74
  -> LEAF_STRONG=EXACT func_80081D74 span=0x84 failures=[none]
```
prior 1w (2.8.1) -> 0
