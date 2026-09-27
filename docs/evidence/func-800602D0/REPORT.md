# func_800602D0 — matching C leaf

- VRAM `0x800602D0`, ROM offset `0X50AD0`, span `0x258`
- Source: `src/func_800602D0.c`; YAML row `[0X50AD0, c, func_800602D0]`
- Build profile: `era_o2_g8_expand_div`

## Levers
number printer; push/pop spelled like func_8005F5B8; x/y locals for x+=9; separate int digit var; i >= digits-1 inline (loop.c hoist -> blez slot gets i=0)

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800602D0
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800602D0
  -> LEAF_STRONG=EXACT func_800602D0 span=0x258 failures=[none]
```
first draft 141 -> 83 -> 2 -> 0 (40w without EXPAND_DIV)
