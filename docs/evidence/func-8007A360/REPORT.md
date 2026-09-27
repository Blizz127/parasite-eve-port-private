# func_8007A360 — matching C leaf

- VRAM `0x8007A360`, ROM offset `0X6AB60`, span `0x6C`
- Source: `src/func_8007A360.c`; YAML row `[0X6AB60, c, func_8007A360]`
- Build profile: `era_o2_g0_cc1_281`

## Levers
CdReset; corrected semantics: a0==1 && func_8007BAC0()!=0 -> 0, otherwise 1 (the prior draft returned 0 for a0!=1, which forced the wrong branch polarity)

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8007A360
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8007A360
  -> LEAF_STRONG=EXACT func_8007A360 span=0x6C failures=[none]
```
prior 4w (2.8.1) / 9-10w (2.7.2) -> 0 on 2.8.1
