# func_800DEA30 — matching C leaf

- VRAM `0x800DEA30`, ROM offset `0XCF230`, span `0x5CC`
- Source: `src/func_800DEA30.c`; YAML row `[0XCF230, c, func_800DEA30]`
- Build profile: `era_o2_g0`

## Levers
FieldEng callback: case 0 actor-position seed (D_800F32D0 template) + func_800CE560(..., func_800DE7A8); case 1 two-iteration spawn loop `i = 0; do { i++; ... } while (i < 2);` (i++ lands in the first jal slot) with rand() % 48 / % 80 offsets rotated through func_800CEAE8; case 2 two effect bursts. Levers: separate locals for the second half (T2, ix2, av2 — sharing T/ix/av with the first D_800F33xx block shifts $a0->$a2 and $v0->$v1: 7 words); `f = &D_800F3374` pointer local held in $s3 for the zero/0x18 stores (func_800D6A1C precedent); w = D_800E1204[idx] + (cond ? 8 : 4) fold.

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800DEA30
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800DEA30
  -> LEAF_STRONG=EXACT func_800DEA30 span=0x5CC failures=[none]
```

