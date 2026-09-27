# func_8007D074 — matching C leaf

- VRAM `0x8007D074`, ROM offset `0X6D874`, span `0xE8`
- Source: `src/func_8007D074.c`; YAML row `[0X6D874, c, func_8007D074]`
- Build profile: `era_o2_g0_fill_epilogue_delay_slot`

## Levers
cc1 2.7.2 (not 2.8.1) fixes the epilogue-restore ordering after trailing global stores; empty register barriers on c=0xC000 (bnez-slot li), on the D_8009B3A0 struct base (keeps la in a register, fed =0xD1 so li a0 precedes it); loop pins i=$3 p=$2; i initialised before p

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8007D074
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8007D074
  -> LEAF_STRONG=EXACT func_8007D074 span=0xE8 failures=[none]
```
2.8.1 lane5 draft 16w -> 2.7.2 41 -> 7 (base barrier) -> 5 -> 3 -> 0
