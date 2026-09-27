# func_8005F1A0 — LINK_EXACT

- VRAM `0x8005F1A0`, file `0x4F9A0`, size `0xDC`
- Profile: `era_o2_g8` (`-O2 -G8`)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_8005F1A0` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_8005F1A0` → `LEAF_STRONG=EXACT func_8005F1A0 span=0xDC vram=0x8005F1A0 compiled=0xE0 failures=[none]`
- Levers (the same measuring loop is inlined in `func_8005F354`):
  - The loop test uses a narrow `unsigned char c = *p` (this gives retail's
    `andi $v1,$a0,0xFF` before `bne`). Inside the body it is copied into a
    **register-pinned** `int ch asm("$4")` so the body re-masks `ch & 0xFF`
    explicitly.
  - `p++` as a separate statement gives the delay-slot increment plus the
    `addiu $s0,-1` exit fix-up.
  - Ladder: plain `int ch` gives 10 mismatches (the test `andi` is missing),
    `unsigned char` everywhere gives 41, and pinning only `c` gives 10.
