# func_8001CE88 — EXACT (lane exeA2, 2026-09-27)

- VRAM `0x8001CE88`, file offset `0xD688`, size `0x2E8` (ends on `jr $ra`; next span `0xD970` = func_8001D170)
- Profile `era_o2_g8_expand_div` (`-O2 -G8`, `MASPSX_EXPAND_DIV=1`)
- YAML: `- [0xD688, c, func_8001CE88]` (replaces `- [0xD688, asm]`; the following row `0xD970` is already a c span)
- Semantics: nearest polyline segment of `p[0..n)` within `D_8009CE2C` of `(x, z)`, returns index or -1.

## Verification (private config build/lanes/exeA2/)
- `python3 tools/build/disc1_preflight.py --config build/lanes/exeA2/disc1.yaml --profiles build/lanes/exeA2/disc1_build_profiles.json --deep --only func_8001CE88`
  → `disc1_preflight: PASS (deep, 2077 c / 195 asm / 2 rodata)`
- `python3 tools/analysis/leaf_strong_check.py --config ... --profiles ... func_8001CE88`
  → `LEAF_STRONG=EXACT func_8001CE88 span=0x2E8 vram=0x8001CE88 compiled=0x2F0 failures=[none]`
- `MASPSX_EXPAND_DIV=1 era_link_check.py src/func_8001CE88.c 0x8001CE88 0x2E8 -O2 -G8` → `word mismatches=0`, `LINK_EXACT`
- profile_necessity.py has no `--config` (it reads the main yaml), so it was checked by hand: default `-O2 -G0` → 154
  mismatches; `-O2 -G8` without EXPAND_DIV → 89 mismatches. Both the flags and the knob are load-bearing.

## Load-bearing lever (closes the exeA PARK "2-word shift")
Retail loads `p[i].x`/`p[i].z` into caller-saved temps, stores the stack-homed `short` prev point from the temp, and copies
into `$s3`/`$s1` just before each axis's first compare. That shape needs:
1. an `int` block temp per axis written copy-first: `{ int t = p[i].x; x1 = t; x0 = t; }` (store-first `x0 = t; x1 = t` → 154)
2. **`short x1, z1`** (not `int`). With `int x1` the compares CSE onto the temp and the copy slides early (158/184 aligned).
   With `short` the copies land where retail has them (0 mismatches).
Ladder: exeA draft (loads straight into s-regs) 154/184 aligned → int temp, int x1 158/184 → int temp, short x1 184/184 exact.
`short t` with `short x1` → 155 (the temp type must be `int`).
