# func_80055724 — LINK_EXACT

- VRAM `0x80055724`, file `0x45F24`, size `0x3C`
- Profile: `era_o2_g8` (`-O2 -G8`), no maspsx env
- Verification (lane-3 private config `$S/lanes/lane3/`):
  - `python3 tools/build/disc1_preflight.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json --deep --only func_80055724` → `disc1_preflight: PASS (deep, …)`
  - `python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json func_80055724` → `LEAF_STRONG=EXACT func_80055724 span=0x3C vram=0x80055724 compiled=0x40 failures=[none]`
- Levers: an unused `int unused[1]` local reproduces retail's empty 8-byte frame
  (`unused[2]` gives 16); `-G8` keeps `D_8009D058`/`D_8009D064` gp-relative.
- Shape: zero `D_8009D064` words of `D_8009D058`.
