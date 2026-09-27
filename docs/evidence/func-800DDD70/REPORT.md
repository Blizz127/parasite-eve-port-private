# func_800DDD70 — matching C leaf

- VRAM `0x800DDD70`, ROM offset `0XCE570`, span `0x338`
- Source: `src/func_800DDD70.c`; YAML row `[0XCE570, c, func_800DDD70]`
- Build profile: `era_o2_g0`

## Levers
FieldEng callback template, first draft exact: case 0 `func_800CE870(D_8009D254, 1, a1); return func_800CE560(..., 0x10, 0x20, func_800DD9E4);`; case 1 spawn (`rand() % 600` magic divide; p[4]=p[5]=0 after p[2]); case 2 reads the actor position three times through `*(unsigned char **)(D_8009D254 + 0x238)` (retail reloads the base after each stack store), mirrors pos into D_800E223C/3E/40, `v = 1; if (D_800E27EC & 1) v = 3;`, byte clear `((unsigned char *)blk)[2] = 0` between the D004C and D0728 calls, then the func_800DC750 D_800F33xx tail with func_800CEDA8(1, ix * 2).

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800DDD70
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800DDD70
  -> LEAF_STRONG=EXACT func_800DDD70 span=0x338 failures=[none]
```

