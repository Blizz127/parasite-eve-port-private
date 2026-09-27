# func_80053D2C — LINK_EXACT

- VRAM `0x80053D2C`, file `0x4452C`, size `0x140`
- Profile: **new** `era_o2_g8_dispatch_8001125c` (`-O2 -G8`,
  `MASPSX_THREE_WORD_SYMBOL_STORE=1`, `MASPSX_DISPATCH_FOLD=jtbl_8001125C`)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_80053D2C` → `dispatch dedup: stripped 72-byte duplicate table …` / `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_80053D2C` → `LEAF_STRONG=EXACT func_80053D2C span=0x140 vram=0x80053D2C compiled=0x140 failures=[none]`
- Levers:
  - The free-slot scan writes a temp `t`, followed by `slot = t;` after the
    scan. This keeps `$a0` live for the `id >= 0x100` arm while `slot` is
    homed in `$s2`.
  - Pins `register int r asm("$17")` and `register unsigned char *rec asm("$4")`
    (without them 36–52 mismatches; pinning `slot` hurts).
  - In the `id >= 0x100` arm only, the polarity is inverted
    (`if (slot >= 0) store; else r = 1;`). This reproduces the cross-jumped
    shared store tail (9 → 0).
  - Explicit case lists 1–18, where 11 falls to default; table
    `jtbl_8001125C` = {1–9→DF8, 10,12–15→E08, 11→E4C, 16–18→E1C}.
