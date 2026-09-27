# func_80055668 — LINK_EXACT

- VRAM `0x80055668`, file `0x45E68`, size `0x80`
- Profile: default `era_o2_g0` (`-O2 -G0`)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_80055668` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_80055668` → `LEAF_STRONG=EXACT func_80055668 span=0x80 vram=0x80055668 compiled=0x80 failures=[none]`
- Levers: indexed compare `p[i * 2] == a0` (a pointer-increment loop swaps the
  `i = 0` / `p = $v0` moves, 2 mismatches); `extern int D_800C0E24[]`.
- Shape: find id in a 20-entry table, set bit `i` in `D_800C0E24`, notify
  `func_8004C4B4(i)`, return found.
