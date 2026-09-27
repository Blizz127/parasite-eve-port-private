# func_800638D8 — matching C leaf

- VRAM `0x800638D8`, ROM offset `0X540D8`, span `0x458`
- Source: `src/func_800638D8.c`; YAML row `[0X540D8, c, func_800638D8]`
- Build profile: `era_o2_g8`

## Levers
gp-relative menu globals (-G8); index-form slot search (w->slot[i]); rp = r pointer local keeps &rect in s2; push/pop via pinned $3/$5 pointers (as func_800634D4), first push through an unpinned local; x = D_8009D124; D_8009D124 = x (retail re-stores it twice); y = m->scroll loaded before the D128 add; separate t = func() & 0x20; f = t != 0 (sltu, not srl/andi) in fresh locals

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800638D8
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800638D8
  -> LEAF_STRONG=EXACT func_800638D8 span=0x458 failures=[none]
```
menu_widget6 draw; 2.7.2 exact
