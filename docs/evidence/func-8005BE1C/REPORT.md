# func_8005BE1C — LINK_EXACT

- VRAM `0x8005BE1C`, file `0x4C61C`, size `0xC0`
- Profile: `era_o2_g8` (`-O2 -G8`)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_8005BE1C` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_8005BE1C` → `LEAF_STRONG=EXACT func_8005BE1C span=0xC0 vram=0x8005BE1C compiled=0xC0 failures=[none]`
- Levers: identical spelling to the copy already inlined in `func_8005D6F4`
  (guarded `do/while` 0xFF fill re-reading `D_8009D0C0 + D_8009D0C4`, then
  `while ((*dst++ = *src++) != 0xFF)`).
