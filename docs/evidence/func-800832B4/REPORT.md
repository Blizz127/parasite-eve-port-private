# func_800832B4 — matching C leaf

- VRAM `0x800832B4`, ROM offset `0X73AB4`, span `0x234`
- Source: `src/func_800832B4.c`; YAML row `[0X73AB4, c, func_800832B4]`
- Build profile: `era_o2_g0_cc1_281_load_delay_nop_before_label`

## Levers
830DC sibling; block-local int w; mode pin $17; sh-mode arm first; natural hoist of timer regs; volatile-cast index read pinned $3 + empty register barrier for retail's reload

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800832B4
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800832B4
  -> LEAF_STRONG=EXACT func_800832B4 span=0x234 failures=[none]
```
41 -> 37 -> 18 (patch 14) -> 2 -> 0; also exact on 2.7.2+p14+p3
