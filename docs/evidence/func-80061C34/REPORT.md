# func_80061C34 — LINK_EXACT (lane exeC, 2026-09-27)

- VRAM `0x80061C34`, file offset `0x52434`, size `0x45C`; yaml `- [0x52434, c, func_80061C34]` replaces
  `- [0x52434, asm]` exactly (next row `[0x52890, c, func_80062090]`).
- Profile `era_o2_g8` (`-O2 -G8`). Framed window draw: outline points pushed onto the `D_8009D148` stack
  (`D_800A22B0`, 12 entries) either from a byte path list (`p != 0`, body of the matched `func_80061B80`) or the four
  box corners (`p == 0`, body of the matched `func_80061A3C`), `func_80061878(…, 0)`; if `a3`, a clip DR_AREA
  (`func_80075B4C`, rect 0,0,0,0), a SPRT (code `0x64`, clut `0x391C`, w×h at the pen) and a second DR_AREA
  (rect 0,0,32,32); always a DR_MODE (`func_80077C84(m, 0, 0, 7)`), each linked into `*D_8009D11C`.
  (The earlier fan-out note calling it a "twin of func_80061044" was wrong — it is a different body.)

## Verification (private lane config `build/lanes/exeC/`)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json --deep --only func_80061C34
  disc1_preflight: PASS (deep, 2051 c / 211 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json func_80061C34
  LEAF_STRONG=EXACT func_80061C34 span=0x45C vram=0x80061C34 compiled=0x460 failures=[none]
```
Necessity (manual): `-O2 -G0` → 264 words + size change; `-G8` load-bearing.

## Levers (ladder, fresh draft)

1. Sibling bodies reused verbatim (`func_80061B80` walk, `func_80061A3C` inline `push()`), pool allocation and
   bitfield `Tag` link as in `func_80061044` → 116.
2. A walker copy `t = p` inside the `p != 0` arm (retail copies `$a2` into `$s0` in the `beqz` slot) → 109;
   a `short *rp = rect;` pointer local for the first DR_AREA call (retail keeps `&rect` in a callee-saved reg) → 82.
3. **A separate pointer variable for the final DR_MODE** (`mm`, not the DR_AREA `m`) plus a separate `rq = rect`
   for the second DR_AREA call: retail gives `{rect ptr, sprite, final prim}` `$s0` and the two DR_AREA prims `$s1`
   → 7.
4. SPRT field order from a greedy line-move search: `x0, clut, w, h, y0, u0, v0` → 0.
