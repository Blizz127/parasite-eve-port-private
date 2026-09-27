# func_8005CAEC — LINK_EXACT

- VRAM `0x8005CAEC`, size `0x1B8`
- Profile: `era_o2_g8`
- Verification (lane-3 private config `$S/lanes/lane3/`):
  - `python3 tools/build/disc1_preflight.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json --deep --only func_8005CAEC` → `disc1_preflight: PASS (deep, …)`
  - `python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json func_8005CAEC` → `LEAF_STRONG=EXACT func_8005CAEC span=0x1B8 … failures=[none]`
- Levers: the loop is written 'i = 0; while (1) { if (i >= D_8009D040) break; ... i++; }' so cc1 keeps retail's unrotated top-tested loop (the for form rotates the test and grows the frame, 85 mismatches); get_sel/clear_bits static inlines as in func_80059534/func_80055E14; list-select prologue inlined; resolver goto-done
- Shape: count list entries whose +0x14 < +0x01 and set their bits in D_8009D058
