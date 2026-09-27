# func_800543CC — LINK_EXACT

- VRAM `0x800543CC`, file `0x44BCC`, size `0x154`
- Profile: `era_o2_g8` (`-O2 -G8`)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_800543CC` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_800543CC` → `LEAF_STRONG=EXACT func_800543CC span=0x154 vram=0x800543CC compiled=0x160 failures=[none]`
- Levers: the twin of `func_800542A0`, using the `func_80059A40` resolver shape
  (`goto done; p = res;` / `else p = 0;`); a short-circuit `&&` chain for the
  four filters (bit, `+0x14` nonzero, `i != skip`). Matched on the first try.
