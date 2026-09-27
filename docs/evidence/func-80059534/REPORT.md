# func_80059534 — LINK_EXACT

- VRAM `0x80059534`, file `0x49D34`, size `0x158`
- Profile: `era_o2_g8` (`-O2 -G8`)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_80059534` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_80059534` → `LEAF_STRONG=EXACT func_80059534 span=0x158 vram=0x80059534 compiled=0x160 failures=[none]`
- Levers:
  - `static inline get_sel()` has a single result local (`if/else`, one
    `return`) and is called twice, so the stack store of `local` lands after the
    join. The two-`return` form duplicates the store (4 mismatches).
  - The resolver is written as `goto done`, with the `D_800C0E20 - 0x1F74`
    (= `D_800BEEAC`) base, as in `func_80059A40`.
