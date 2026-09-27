# func_80076434 — matching C leaf

- VRAM `0x80076434`, ROM offset `0X66C34`, span `0x230`
- Source: `src/func_80076434.c`; YAML row `[0X66C34, c, func_80076434]`
- Build profile: `era_o2_g0_cc1_281_fill_call_delay_slot`

## Levers
lane4 draft; empty register barrier on a ret=0 local keeps the return-0 move before the epilogue restores (2.8.1 sched2 sinks a constant return below them)

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_80076434
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_80076434
  -> LEAF_STRONG=EXACT func_80076434 span=0x230 failures=[none]
```
5w -> 0 (9w without patch 8)
