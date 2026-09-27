# func_8005DF6C — LINK_EXACT

- VRAM `0x8005DF6C`, file `0x4E76C`, size `0xCC`
- Profile: `era_o2_g8` (`-O2 -G8`)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_8005DF6C` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_8005DF6C` → `LEAF_STRONG=EXACT func_8005DF6C span=0xCC vram=0x8005DF6C compiled=0xD0 failures=[none]`
- Levers: test the global directly (`if (D_8009D0E0 != 0) { p = D_8009D0E0; …`)
  so cc1 loads into `$v0` and copies to `$a2` in the delay slot; a `while`
  loop with an inner null `break` keeps retail's redundant post-loop `if (p)`;
  12-byte struct copy `*out = *p`.
- Shape: unlink the first list node whose `+4` word matches the mask, push it on
  the free list `D_8009D0DC`, and copy it out (zero `out` fields if none match).
