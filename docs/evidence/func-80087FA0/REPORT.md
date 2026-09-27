# func_80087FA0 — matching C leaf

- VRAM `0x80087FA0`, ROM offset `0X787A0`, span `0x3A4`
- Source: `src/func_80087FA0.c`; YAML row `[0X787A0, c, func_80087FA0]`
- Build profile: `era_o2_g0`

## Levers
reuse one int n for sum and all LFO results (retail a1); block-local unsigned d for (x&0x7F00)>>8 with per-arm products; tremolo split e/n; block-local q=m>>7 + empty register barrier keeps srl above the 0x8E lhu

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_80087FA0
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_80087FA0
  -> LEAF_STRONG=EXACT func_80087FA0 span=0x3A4 failures=[none]
```
Spu voice envelope/LFO tick; 2.8.1 band but 2.7.2 exact (2.8.1 best 2: epilogue slot + srl order)
