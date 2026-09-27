# func_800DC5BC — matching C leaf

- VRAM `0x800DC5BC`, ROM offset `0XCCDBC`, span `0x194`
- Source: `src/func_800DC5BC.c`; YAML row `[0XCCDBC, c, func_800DC5BC]`
- Build profile: `era_o2_g0`

## Levers
FieldEng template (func_800DC910/func_800DBCD8 siblings). Closes lane6's PARK (3-4-word $v0/$v1 swap between the `li 4` compare constant and the idx*2 index temp): fold the weapon-table term into one expression, `w = D_800E1204[idx] + ((idx == 4 && D_800F3428 != 0) ? 6 : 2);`. The if/else, a separate `w += cond ? 6 : 2`, inverted polarity, `unsigned short idx` and byte-offset indexing all stay at 4-7 words.

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800DC5BC
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800DC5BC
  -> LEAF_STRONG=EXACT func_800DC5BC span=0x194 failures=[none]
```

