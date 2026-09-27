# func_800542A0 — LINK_EXACT

- VRAM `0x800542A0`, file `0x44AA0`, size `0x12C`
- Profile: `era_o2_g8` (`-O2 -G8`)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_800542A0` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_800542A0` → `LEAF_STRONG=EXACT func_800542A0 span=0x12C vram=0x800542A0 compiled=0x130 failures=[none]`
- Levers: the slot resolver is a `goto done` with an in-range guard, as in
  `func_80058C4C`. The shift amount goes through an `if/else` into a named `sh`
  local, which reproduces retail's `addu $v0,$zero,$zero; srav`. A `?:` inside
  the shift expression duplicates the test instead (28 mismatches).
