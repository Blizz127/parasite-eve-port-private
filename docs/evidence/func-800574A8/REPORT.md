# func_800574A8 — LINK_EXACT

- VRAM `0x800574A8`, size `0x1AC`
- Profile: `era_o2_g8`
- Verification (lane-3 private config `$S/lanes/lane3/`):
  - `python3 tools/build/disc1_preflight.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json --deep --only func_800574A8` → `disc1_preflight: PASS (deep, …)`
  - `python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json func_800574A8` → `LEAF_STRONG=EXACT func_800574A8 span=0x1AC … failures=[none]`
- Levers: macros CAP(p) = (lvl > 999 ? 999 : lvl) with lvl = p[9] + *(short*)(p+0x12), HP(p) = *(u16*)(p+0xA), and MIN(a,b) = a < b ? a : b; amt = MIN(CAP(p) - HP(p), HP(q)). The macro re-evaluation makes fold distribute the compare into both CAP arms and re-expand at the join, as in retail; the '< 1000 ? lvl : 999' spelling is 15 words shorter. Slot selection uses 't >= 0x13' first; the DB44 arm takes 'goto out' with p assigned directly
- Shape: transfer HP from the potion slot q to the selected record, capped at max
