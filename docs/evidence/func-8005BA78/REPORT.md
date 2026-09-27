# func_8005BA78 — LINK_EXACT

- VRAM `0x8005B91C`, file `0x4C278`, size `0x16C`
- Profile: `era_o2_g0_expand_div` (`-O2 -G0`, `MASPSX_EXPAND_DIV=1`). The knob is
  load-bearing: without it, 29 words mismatch.
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_8005BA78` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_8005BA78` → `LEAF_STRONG=EXACT func_8005BA78 span=0x16C … failures=[none]`
- Levers: a binary search written as `do { … step >>= 1; } while (step != 0)`
  over `t = func_8005DB8C(a0)`. The clamp is spelled `i = (i > 0x62) ? 0x62 : i;`,
  which gives retail's default-then-override `$v1` (the `if` form costs 52
  words). The fraction is `((val - t[i]) * 49) / (t[i+1] - t[i])`, clamped to 48.
- Twin of `func_8005B91C`. It computes `rem = (i < 0x62) ? t[i+1] - val : 0`
  before the fraction branch, has no fraction clamp, and stores `*outRem` and
  `*outFrac` (0x30 when `i >= 0x62`).
