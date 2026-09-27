# func_800D7764 — matching C leaf

- VRAM `0x800D7764`, ROM offset `0XC7F64`, span `0x2B8`
- Source: `src/func_800D7764.c`; YAML row `[0XC7F64, c, func_800D7764]`
- Build profile: `era_o2_g0`

## Levers
FieldEng callback template (case 0 D_8009D254 call, case 1 `if (T < 0x3C) break; return 1;`, case 2 D_800F33xx block). Closing lever: the weapon-table term folded into ONE expression `w = D_800E1204[idx] + ((idx == 4 && D_800F3428 != 0) ? 6 : 2);` with `s0v = 0x2800;` written between `idx = D_800F336C;` and it. The if/else form (orphan draft) leaves 14 words (the /32 `sra $s2` scheduled late, `li 4` in $v1); the fold with s0v after it costs 154.

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800D7764
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800D7764
  -> LEAF_STRONG=EXACT func_800D7764 span=0x2B8 failures=[none]
```
Orphan draft (lane6) was recorded as 0 mismatches but measured 14 here.
