# func_800D71B8 — matching C leaf

- VRAM `0x800D71B8`, ROM offset `0XC79B8`, span `0x364`
- Source: `src/func_800D71B8.c`; YAML row `[0XC79B8, c, func_800D71B8]`
- Build profile: `era_o2_g0`

## Levers
Sibling of func_800D7FBC/func_800D8E74. Case 1: `q = D_800E27EC * 700 / 36;` first (the T load + magic mult lead retail), then `*(short *)((char *)a1 + 4) += 1;`, then `v = D_800E21E2 - 700; a1[4] = q + v;` (inline `D + q - 700` reassociates: 16 words; `q + (D - 700)` inline narrows the -700 to HImode 0xFD44: 186 words). Case 2: nested switch on a1[0] with `s2v = 0x40` assigned inside case 0 and case 1 (reorg steals `li s2,64` / `li s1,4096` from the case targets into the beqz/beq slots; a shared pre-switch s2v costs 3 words). Tail `s1v = s1v * 3 / 2`, w + 7 : 3.

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800D71B8
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800D71B8
  -> LEAF_STRONG=EXACT func_800D71B8 span=0x364 failures=[none]
```

