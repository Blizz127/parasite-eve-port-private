# func_8006BECC — matching C leaf

- VRAM `0x8006BECC`, ROM offset `0X5C6CC`, span `0x300`
- Source: `src/func_8006BECC.c`; YAML row `[0X5C6CC, c, func_8006BECC]`
- Build profile: `era_o2_g0_dispatch_800113b0`

## Levers
goto-top state machine (no loop.c hoists); D_800930D8/DA with k=area+3/+8; indexed loop bodies; unused int pad[4] reproduces retail's 16 extra reserved frame bytes (vars=32, never stored) - flagged for review

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8006BECC
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8006BECC
  -> LEAF_STRONG=EXACT func_8006BECC span=0x300 failures=[none]
```
191 -> 158 (goto top) -> 119 (k index) -> 22/21/18 (indexed loops) -> 0 (frame pad); new private profile era_o2_g0_dispatch_800113b0
