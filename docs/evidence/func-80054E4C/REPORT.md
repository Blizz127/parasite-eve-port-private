# func_80054E4C — LINK_EXACT

- VRAM `0x80054E4C`, file `0x4564C`, size `0x10C`
- Profile: `era_o2_g8_force_d800c0e0c_absolute` (`-O2 -G8`,
  `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800C0E0C`)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_80054E4C` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_80054E4C` → `LEAF_STRONG=EXACT func_80054E4C span=0x10C vram=0x80054E4C compiled=0x110 failures=[none]`
- Levers: same statement order as the inlined block in `func_80059A40`:
  compute the ternary into `c2`, then `used = 0`, then the `D_8009D048` store,
  then `cap = c2`. This puts `used = 0` in the `beqz` slot and the cap move in
  the `jal` slot (the direct form gives 5 mismatches).
