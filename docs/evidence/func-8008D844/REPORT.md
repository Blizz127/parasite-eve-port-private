# func_8008D844 — matching C leaf

- VRAM `0x8008D844`, ROM offset `0X7E044`, span `0x338`
- Source: `src/func_8008D844.c`; YAML row `[0X7E044, c, func_8008D844]`
- Build profile: `era_o2_g0`

## Levers
Seq/Voice structs; goto voice loop (no biased giv); bit reused as the 24-counter

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8008D844
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8008D844
  -> LEAF_STRONG=EXACT func_8008D844 span=0x338 failures=[none]
```
110 -> 77 -> 70 -> 11 -> 0
