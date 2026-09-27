# func_80057F14 — LINK_EXACT

- VRAM `0x80057F14`, file `0x48714`, size `0x11C`
- Profile: default `era_o2_g0` (`-O2 -G0`; `-O2 -G8` also matches, since there are no gp refs)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_80057F14` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_80057F14` → `LEAF_STRONG=EXACT func_80057F14 span=0x11C vram=0x80057F14 compiled=0x120 failures=[none]`
- Levers: a natural `if/else-if` resolver chain (the unreachable `0x200` test
  stays in the `< 0x200` arm); `short D_800A1FD4[]` indexed load. Matched on
  the first try.
