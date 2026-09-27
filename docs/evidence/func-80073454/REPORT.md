# func_80073454 — matching C leaf

- VRAM `0x80073454`, ROM offset `0X63C54`, span `0x100`
- Source: `src/func_80073454.c`; YAML row `[0X63C54, c, func_80073454]`
- Build profile: `era_o2_g0_fill_epilogue_delay_slot`

## Levers
lane4 draft unchanged; cc1 2.7.2 + patch 3 instead of 2.8.1 removes the 2-word zero-register CSE residual

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_80073454
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_80073454
  -> LEAF_STRONG=EXACT func_80073454 span=0x100 failures=[none]
```
2.8.1 2w -> 2.7.2+p3 0 (2.7.2 w/o patch 3: 2w)
