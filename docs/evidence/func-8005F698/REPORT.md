# func_8005F698 — LINK_EXACT

- VRAM `0x8005F698`, size `0x1AC`
- Profile: `era_o2_g8`
- Verification (lane-3 private config `$S/lanes/lane3/`):
  - `python3 tools/build/disc1_preflight.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json --deep --only func_8005F698` → `disc1_preflight: PASS (deep, …)`
  - `python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json func_8005F698` → `LEAF_STRONG=EXACT func_8005F698 span=0x1AC … failures=[none]`
- Levers: push/pop of the D_8009D12C position stack as in func_8005F354 (the pop uses a second pointer local 'sq'; reusing sp or naming x/y temporaries perturbs the push); the glyph-measure body keeps the raw byte in c and the value in v (not the func_8005F1A0 pinned-ch shape); the move-down is '{ int *q = &D_8009D124; *q = D_8009D124; D_8009D128 += 0xE; }' (inlined func_8005E8A4(0, 0xE)), which keeps the no-op D124 store first
- Shape: word-wrapped text print: draw glyphs until width is exceeded, then newline (+0xE)
