# func_80061A3C — LINK_EXACT

- VRAM `0x80061A3C`, file `0x5223C`, size `0x144`
- Profile: `era_o2_g8` (`-O2 -G8`)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_80061A3C` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_80061A3C` → `LEAF_STRONG=EXACT func_80061A3C span=0x144 vram=0x80061A3C compiled=0x150 failures=[none]`
- Levers: a `static inline push(x, y)` with the same body as the loop in
  `func_80061B80`. The bound is written as `D_800A22B0 + 0x18`, not as a
  `D_800A22E0` symbol, so the first push folds to `base + 0x30` exactly as in
  retail (75 mismatches with the separate symbol).
- Shape: push the 4 rectangle corners onto the `D_8009D148` point stack, then
  call `func_80061878(D_800930A8, a2)`.
