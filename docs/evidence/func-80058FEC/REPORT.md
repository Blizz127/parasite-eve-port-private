# func_80058FEC — EXACT (lane exeC3, 2026-09-27)

- VRAM `0x80058FEC`, file offset `0x497EC`, size `0x548` (compiled object 0x550 = body + gas pad).
- Profile `era_o2_g8_force_d800c0e20_d800c0e22_absolute` (`-O2 -G8`, `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800C0E20,D_800C0E22`).
  Necessity: `-O2 -G0` → 314 word mismatches; plain `era_o2_g8` → leaf_strong_check link failure
  `small-data section too large` (the FORCE list is load-bearing at full link).
- YAML: `- [0x497EC, c, func_80058FEC]` (next row `[0x49D34, c, func_80059534]` unchanged).

## Verify (private lane config build/lanes/exeC3/)
- `disc1_preflight.py --config build/lanes/exeC3/disc1.yaml --profiles build/lanes/exeC3/disc1_build_profiles.json --deep --only func_80058FEC` → `disc1_preflight: PASS (deep, ...)`
- `leaf_strong_check.py ... func_80058FEC` → `LEAF_STRONG=EXACT func_80058FEC span=0x548 vram=0x80058FEC compiled=0x550 failures=[none]`
- `era_link_check.py src/func_80058FEC.c 0x80058FEC 0x548 -O2 -G8` (+FORCE env) → `word mismatches=0`, `LINK_EXACT`

## Levers
Built on the exeC draft (inline slot() ternary, inline item() goto-done, xor swap inline).
Closing lever (exeC3): the whole 0x33/0x33 arm is an inline `swapi(int x, int y)` (swap + the two
D_800C0E20/D_800C0E22 fix-ups) called as `swapi(slot(b), slot(d))`. Inline-call argument
evaluation gives the first slot() result its own pseudo (retail `lh v0` / separate `v0 = 0`
block, then `move a1,v0` → param copy `move a3,a1`). Ladder: `x = slot(b); y = slot(d);` in the
arm (8 aligned insns / 271 raw), `swapi(x, y)` with named locals (same 8), slot() in statement
forms (8–11), `swapi(slot(b), slot(d))` → 0.
