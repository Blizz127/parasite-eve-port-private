# func_8005EB64 — LINK_EXACT

- VRAM `0x8005EB64`, size `0x1B4`
- Profile: `era_o2_g8`
- Verification (lane-3 private config `$S/lanes/lane3/`):
  - `python3 tools/build/disc1_preflight.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json --deep --only func_8005EB64` → `disc1_preflight: PASS (deep, …)`
  - `python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json func_8005EB64` → `LEAF_STRONG=EXACT func_8005EB64 span=0x1B4 … failures=[none]`
- Levers: NEW LEVER: the PSY-Q addPrim is written through a P_TAG bitfield ('struct { unsigned addr:24; unsigned len:8; }': p->addr = ot->addr; ot->addr = (u32)p). This lets cc1 schedule the tag load early exactly as retail does (the explicit '& 0xFF000000 | & 0xFFFFFF' spelling gives 30 mismatches). Primitive allocation uses the func_8005EA8C 'D_8009D100 + n < (u32*)(D_8009D104 + 0x4000)' form
- Shape: emit one glyph SPRT (0x14) + DR_TPAGE (8) into the OT
