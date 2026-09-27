# func_80058AA8 — LINK_EXACT

- VRAM `0x80058AA8`, file `0x492A8`, size `0x114`
- Profile: `era_o2_g8` (`-O2 -G8`)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_80058AA8` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_80058AA8` → `LEAF_STRONG=EXACT func_80058AA8 span=0x114 vram=0x80058AA8 compiled=0x120 failures=[none]`
- Levers: the id→record resolver is written as `goto done` into `res`, then
  `p = res`, as in `func_80059A40` and `func_80058C4C`. An `if/else-if` chain
  gives 29 mismatches. A pointer local `q += 0x10` handles the `D_800C20A4`
  window.
