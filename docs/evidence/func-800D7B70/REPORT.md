# func_800D7B70 — matching C leaf

- VRAM `0x800D7B70`, ROM offset `0XC8370`, span `0x308`
- Source: `src/func_800D7B70.c`; YAML row `[0XC8370, c, func_800D7B70]`
- Build profile: `era_o2_g0`

## Levers
FieldEng callback template (func_800DACA4 sibling; case 0 returns func_800CE560(..., func_800D7A1C); case 2 tail is the func_800DC750 D_800F33xx block with D_800E11E6 and a one-arg func_800CEDA8(1)). Load-bearing: in case 2 the /8 quotient is taken into a local first, then a fresh `u = D_800E27EC` read, then the `*(int *)(a1+6)` store, so the int store lands in the func_800CF3AC jal slot after the global reload (reading the global before the division hoists the load above the bgez: 8 words; passing D_800E27EC directly: 5 words). Case 1 uses the `+= (rand() & 0x1F) + 0x8AA` spelling from func_800DACA4.

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800D7B70
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800D7B70
  -> LEAF_STRONG=EXACT func_800D7B70 span=0x308 failures=[none]
```

