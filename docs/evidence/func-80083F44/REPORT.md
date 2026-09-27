# func_80083F44 — matching C leaf

- VRAM `0x80083F44`, ROM offset `0X74744`, span `0xD8`
- Source: `src/func_80083F44.c`; YAML row `[0X74744, c, func_80083F44]`
- Build profile: `era_o2_g0_cc1_281`

## Levers
tail as if (a0[0x36] != 0) return f(a0, a0[0x36]); return f(a0, 0x42); (cross-jump merges the jal, arg reload + j-slot li as retail)

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_80083F44
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_80083F44
  -> LEAF_STRONG=EXACT func_80083F44 span=0xD8 failures=[none]
```
prior 6-9w -> 0
