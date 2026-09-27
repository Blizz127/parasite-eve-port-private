# func_800579D4 — LINK_EXACT

- VRAM `0x800579D4`, size `0x19C`
- Profile: `era_o2_g8`
- Verification (lane-3 private config `$S/lanes/lane3/`):
  - `python3 tools/build/disc1_preflight.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json --deep --only func_800579D4` → `disc1_preflight: PASS (deep, …)`
  - `python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json func_800579D4` → `LEAF_STRONG=EXACT func_800579D4 span=0x19C … failures=[none]`
- Levers: a single 'int n' out-param local for func_800515F8 (retail's 0x28 frame; int[3] gives 0x30); the selected record is resolved with the func_80059A40 goto-done shape (D_800C0E22 - 0x1F76 base); the attribute-0xE scan uses (p + i)[0x15] with the p[0x14] bound re-read
- Shape: stat modifier: ids 6/0x13 pass through a1, id 5 = measured/3, else table value ×2/3 if the equipped item has attribute 0xE
