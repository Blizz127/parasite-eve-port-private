# func_80057834 — LINK_EXACT

- VRAM `0x80057834`, size `0x1A0`
- Profile: `era_o2_g8_dispatch_800112a4`
- Verification (lane-3 private config `$S/lanes/lane3/`):
  - `python3 tools/build/disc1_preflight.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json --deep --only func_80057834` → `disc1_preflight: PASS (deep, …)`
  - `python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json func_80057834` → `LEAF_STRONG=EXACT func_80057834 span=0x1A0 … failures=[none]`
- Levers: NEW profile era_o2_g8_dispatch_800112a4 (-O2 -G8, THREE_WORD + DISPATCH_FOLD=jtbl_800112A4); resolver goto-done form; explicit case lists (1, 2, 4-6, 12-14; 3 and 7-11 fall to default) give the 14-entry table; a single int local for func_800512AC's out pointer (0x20 frame); matched on the first compile
- Shape: use/undo item: log an undo record when D_8009D028, else dispatch on the record's type byte +0xE
