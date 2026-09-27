# func_8004D18C — LINK_EXACT

- VRAM `0x8004D18C`, file `0x3D98C`, size `0xF0`
- Profile: `era_o2_g8` (`-O2 -G8`)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_8004D18C` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_8004D18C` → `LEAF_STRONG=EXACT func_8004D18C span=0xF0 vram=0x8004D18C compiled=0xF0 failures=[none]`
- Levers: the twin of `func_8004D084` with `a0 = 1` inlined. A named local
  `a0 = 1;` assigned just before the `+0x48 == 2` test keeps the constant in
  `$s0` (`li $s0,1` in the `bne` delay slot). A bare `1` costs 9 words.
