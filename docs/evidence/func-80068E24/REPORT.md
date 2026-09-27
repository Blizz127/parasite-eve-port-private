# func_80068E24 — matching C leaf

- VRAM `0x80068E24`, ROM offset `0X59624`, span `0x328`
- Source: `src/func_80068E24.c`; YAML row `[0X59624, c, func_80068E24]`
- Build profile: `era_o2_g0_expand_div`

## Levers
Fade struct; PsyQ addPrim via P_TAG bitfields; len via P_TAG; byte-offset DR_MODE address for the code store

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_80068E24
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_80068E24
  -> LEAF_STRONG=EXACT func_80068E24 span=0x328 failures=[none]
```
25 -> 1 (addPrim bitfields) -> 0
