# func_80066268 — matching C leaf

- VRAM `0x80066268`, ROM offset `0X56A68`, span `0x338`
- Source: `src/func_80066268.c`; YAML row `[0X56A68, c, func_80066268]`
- Build profile: `era_o2_g0_expand_div`

## Levers
early reads in retail order; cos products before stores; pointer-to-volatile D_800B1624; Room struct; int temporaries stop short-store narrowing (lh vs lhu)

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_80066268
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_80066268
  -> LEAF_STRONG=EXACT func_80066268 span=0x338 failures=[none]
```
190 -> 112 -> 86 -> 25 -> 3 -> 0
