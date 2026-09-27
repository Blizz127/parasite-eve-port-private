# func_800DACA4 — matching C leaf

- VRAM `0x800DACA4`, ROM offset `0XCB4A4`, span `0x2E8`
- Source: `src/func_800DACA4.c`; YAML row `[0XCB4A4, c, func_800DACA4]`
- Build profile: `era_o2_g0`

## Levers
FieldEng callback template (func_800DC750 / func_800D6A1C siblings): two B4 unaligned block copies (D_800C22E8/EC) at entry; case 0 `t = rand(); q = D_8009D254; *(int *)(a1+4) = t; func_800CE870(q, 0, a1); return func_800CE560(...)`; case 1 spawn via func_800CE610 with `*(int *)(a1+4) += (rand() & 0x1F) + 0x8AA;` (the `x + 0x8AA + (r & 0x1F)` spelling reassociates the constant onto the rand term: 3 words); case 2 `T = D_800E27EC` local, /80 magic divides, D_800D004C x2 + D_800D0728. First draft 3 words, closed by the compound-assignment spelling.

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800DACA4
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800DACA4
  -> LEAF_STRONG=EXACT func_800DACA4 span=0x2E8 failures=[none]
```

