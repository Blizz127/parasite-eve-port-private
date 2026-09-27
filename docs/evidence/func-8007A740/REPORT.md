# func_8007A740 — matching C leaf

- VRAM `0x8007A740`, ROM offset `0X6AF40`, span `0x14C`
- Source: `src/func_8007A740.c`; YAML row `[0X6AF40, c, func_8007A740]`
- Build profile: `era_o2_g0_fill_branch_macro_split_epilogue`

## Levers
CdControlB: A60C shape (one/neg1 invariant locals, u_char a0, base pin $3, saved pinned $21) + err=0 on success, tail 'if(err) return 0; return func_8007B010(0,a2)==2'

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8007A740
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8007A740
  -> LEAF_STRONG=EXACT func_8007A740 span=0x14C failures=[none]
```
first-try from A60C template, 0 mismatches
