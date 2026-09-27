# func_800D7FBC — matching C leaf

- VRAM `0x800D7FBC`, ROM offset `0XC87BC`, span `0x3CC`
- Source: `src/func_800D7FBC.c`; YAML row `[0XC87BC, c, func_800D7FBC]`
- Build profile: `era_o2_g0_expand_div`

## Levers
Sibling of func_800D8978 (B4 blk copy of D_800C22E0; w = D_800E1204[idx] + ((idx == 4 && D_800F3428 != 0) ? 7 : 3) fold; s1v <<= 1 before the idx load). Case 1 has a real `D_800E27EC % v` (v = 3, or 1 when *(short *)(D_800E2368 + 0x1E) == 0xB) whose break guards need MASPSX_EXPAND_DIV=1 (plain era_o2_g0: 147 words). Case 2 is a nested `switch (a1[0]) { case 0; case 1; default }` whose shared `a1[2] = 0; a1[0] = n;` tail is cross-jumped by cc1. First draft 97 words only because of a /6 vs /12 divisor slip (0x2AAAAAAB + sra 1).

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800D7FBC
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800D7FBC
  -> LEAF_STRONG=EXACT func_800D7FBC span=0x3CC failures=[none]
```

