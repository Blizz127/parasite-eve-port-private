# func_8004B394 — LINK_EXACT

- VRAM `0x8004B394`, size `0x1A0`
- Profile: `era_o2_g8`
- Verification (lane-3 private config `$S/lanes/lane3/`):
  - `python3 tools/build/disc1_preflight.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json --deep --only func_8004B394` → `disc1_preflight: PASS (deep, …)`
  - `python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json func_8004B394` → `LEAF_STRONG=EXACT func_8004B394 span=0x1A0 … failures=[none]`
- Levers: the two channel arms compute m = col & ~(0xFF << sh) first, then c, then a separate x = (clamped c) << sh (spelled '(c > 0xE8) ? 0xE8 : c' and '(c < 0x20) ? 0x20 : c'), then share one func_800614AC(m | x) tail; with x folded into the call, the sh/ev callee-saved homes swap (76-88 mismatches); the 0x10000 arm uses 'goto keep' so s==0 skips the 0x404040 reset
- Shape: colour-picker page input handler (±2 per channel, clamp 0x20..0xE8)
