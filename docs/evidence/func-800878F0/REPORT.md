# func_800878F0 — matching C leaf

- VRAM `0x800878F0`, ROM offset `0X780F0`, span `0x1B8`
- Source: `src/func_800878F0.c`; YAML row `[0X780F0, c, func_800878F0]`
- Build profile: `era_o2_g0`

## Levers
Akao per-voice SPU register flush: a dirty-flag word at +4 gates eight register writers (func_800877BC / 87798 / 877D4 / 877F0 / 8788C / 8780C / 878C0 / 8783C+87864); each writer clears its bits with `if ((a1->flags &= ~MASK) == 0) return;`, the last pair falls through to `a1->flags = 0`. Record typed as a struct (u32 words 0..0x18, u16 0x1C..0x26, s16 0x28/0x2A for the lh pair). Plain 2.7.2 even inside the 2.8.1 band (2.8.1: 2 words). First draft exact.

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800878F0
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800878F0
  -> LEAF_STRONG=EXACT func_800878F0 span=0x1B8 failures=[none]
```

