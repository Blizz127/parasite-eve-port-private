# func_80089784 — matching C leaf

- VRAM `0x80089784`, ROM offset `0X79F84`, span `0x1DC`
- Source: `src/func_80089784.c`; YAML row `[0X79F84, c, func_80089784]`
- Build profile: `era_o2_g0`

## Levers
Key-off sibling of the func_80089980 flush triplet: same Trk struct view of D_8009D2C8 (fields 0x6C & 0x80 / 0x70 / 0x4 & 0x18 / 0x8), same temporary +0x68 advance around func_80089724. Extra per-block `f18 &= ~f8` clears: in the second block the clear must be written BEFORE `s0v &= ~q->f8` (the other order lets cse reuse the f8 load: 55 words); the finished mask ORs in and clears D_800BCD5C and is passed to func_80087728 when non-zero. Plain 2.7.2.

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_80089784
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_80089784
  -> LEAF_STRONG=EXACT func_80089784 span=0x1DC failures=[none]
```

