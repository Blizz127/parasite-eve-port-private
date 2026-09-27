# func_8004CC50 — LINK_EXACT

- VRAM `0x8004CC50`, file `0x3D450`, size `0x15C`
- Profile: `era_o2_g8` (`-O2 -G8`)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_8004CC50` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_8004CC50` → `LEAF_STRONG=EXACT func_8004CC50 span=0x15C … failures=[none]`
- Levers: the text buffer is written inline as `D_800A1A20 + (has << 6)` at
  every use, and CSE homes it in `$s0` after the first `func_8005DC4C` call. A
  named `buf` local is computed before that call, which also swaps the
  `has`/`a1` callee-saved homes (76 mismatches, and pinning does not fix it).
  `w = (f(buf) < 100) ? 100 : f(buf)` calls the function twice, as retail does.
