# func_80054CF8 — LINK_EXACT

- VRAM `0x80054CF8`, file `0x454F8`, size `0x154`
- Profile: `era_o2_g8_force_d800c0e0c_absolute` (`-O2 -G8`,
  `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800C0E0C`)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_80054CF8` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_80054CF8` → `LEAF_STRONG=EXACT func_80054CF8 span=0x154 vram=0x80054CF8 compiled=0x160 failures=[none]`
- Levers: a `for (; D_8009D06C > 0; D_8009D06C--)` loop. `s = D_800C0E20;` sits
  at the top of the body: it is loop-invariant, so cc1 hoists it after the entry
  guard. Placed before the loop it lands before the `blez`; placed inside the
  `if` it is not hoisted. The free-slot scan writes a temp `t`, then
  `slot = t`.
