# func_80089328 — PARKED (lane exeD, 2026-09-27)

VRAM `0x80089328`, file `0x79b28`, size `0x3fc`, `asm/disc1/79B28.s`.

- Best: 34 words (archived) under 2.7.2.
- Draft: `build/lanes/exeD/drafts/func_80089328.c` (if present).
- Re-checked: the archived residual is a pure s1/s2 callee-saved swap (the draft reproduces it). Not worked further this round.

## lane exeE (2026-09-27)
- Draft `build/lanes/exeE/drafts/func_80089328.c`: 34 → **32** with `register unsigned int b asm("$17")`
  (m then lands in s2 naturally, but the pinned b ties the `fC` load into s1).
- Tried: m/d/bit/b/v all pinned (194), m pinned `$18` (255), d merged into m (34), bit merged too (41),
  e merged into m (42), `register` without asm (34), b via temp + barrier (32/40).
- Residual is global-alloc priority: retail b (and v) outrank m (and d, bit).
