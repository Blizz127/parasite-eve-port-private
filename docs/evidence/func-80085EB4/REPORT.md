# func_80085EB4 — matching C leaf

- VRAM `0x80085EB4`, ROM offset `0X766B4`, span `0x5C`
- Source: `src/func_80085EB4.c`; YAML row `[0X766B4, c, func_80085EB4]`
- Build profile: `era_o2_g0`

## Levers
Range-checked SPU register read: `(unsigned)(a0 - 0x1010) > 0x7EFE8` gives lui/ori + sltu; `D_8009B414` declared `volatile unsigned short` reproduces retail's store-then-reload (non-volatile folds to andi 0xFFFF, 11-13 words). Plain 2.7.2 (2.8.1 fills the epilogue slot: 2 words). Trailing nop at 0x76710 stays its own asm row (deep-boundary).

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_80085EB4
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_80085EB4
  -> LEAF_STRONG=EXACT func_80085EB4 span=0x5C failures=[none]
```

