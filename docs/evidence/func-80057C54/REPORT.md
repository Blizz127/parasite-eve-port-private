# func_80057C54 — LINK_EXACT

- VRAM `0x80057C54`, file `0x48454`, size `0xC4`
- Profile: `era_o2_g8` (`-O2 -G8`); patch 3 was tested and is NOT needed
  (0 mismatches without it)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_80057C54` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_80057C54` → `LEAF_STRONG=EXACT func_80057C54 span=0xC4 vram=0x80057C54 compiled=0xD0 failures=[none]`
- Levers: `-G8` for `D_8009D048`; `signed char D_800C0E20[]`/`D_800C0E22[]`
  arrays give a `lui/addiu` base plus `lb`; three-xor swap; `if / else if`
  selection fixups.
