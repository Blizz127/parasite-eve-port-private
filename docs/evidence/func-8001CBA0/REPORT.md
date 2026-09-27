# func_8001CBA0 — LINK_EXACT (lane exeA, 2026-09-27)

- VRAM `0x8001CBA0`, file offset `0xD3A0`, size `0x2E8` (ends on `jr $ra` at `0x8001CE80` + delay slot; next function `func_8001CE88` at `0xD688`)
- Profile `era_o2_g8_expand_div` (`-O2 -G8`, `MASPSX_EXPAND_DIV=1`)
- YAML: `- [0xD3A0, c, func_8001CBA0]` + `- [0xD688, asm]` (replaces `- [0xD3A0, asm]`)

Shape: wall-segment push-out. Takes the polyline point `p[i]` and its predecessor
(`p[i-1]`, wrapping to `p[n-1]` when `i <= 0`), normalises the segment direction
(`func_80078134`), projects the object position (`o->f28/f30` relative to `o->f40/f48`)
onto it with four `func_8003708C` fixed-point multiplies, then computes the
perpendicular distance `|((z>>16)-z1)*dx - ((x>>16)-x1)*dz| / len` (`len` =
`func_80078004(dx*dx+dz*dz)`); if it exceeds `D_8009CE2C` it stores the projected
point, else it nudges by `k<<16` in the four axis directions (`switch (j)`) until one
clears. Sibling of the parked `func_8001C7DC`.

## Verification (private lane config)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json --deep --only func_8001CBA0
  disc1_preflight: PASS (deep, 2043 c / 217 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json func_8001CBA0
  LEAF_STRONG=EXACT func_8001CBA0 span=0x2E8 vram=0x8001CBA0 compiled=0x2F0 failures=[none]
```
(`compiled=0x2F0` is the 8-byte zero gas pad; `nonzero_pad=0`.)
`profile_necessity.py` has no `--config/--profiles` (hard-wired to main config) — skipped.
Manual necessity: `-O2 -G8` without `MASPSX_EXPAND_DIV` → 83 mismatches; `-O2 -G0` with it → 88.

## Levers (mismatch ladder)

1. First draft (pointer `q = &p[i]; if (i <= 0) q = &p[n]; x0 = q[-1].x`) → 57:
   whole callee-saved allocation rotated (x0/z0 took `$s0/$s1`).
2. **Predecessor loaded in both arms of an if/else** (`if (i > 0) { x0 = p[i-1].x; z0 = p[i-1].z; } else { x0 = p[n-1].x; ... }`)
   → 9. The pseudos now span blocks, so global-alloc (not local-alloc) places them and
   retail's `$s3`=o, `$s4/$s5`=x0/z0 allocation falls out; jump2 cross-jumps the two
   `lh -6/-2($v0)` tails into retail's shared block.
3. **`px += o->f40; pz += o->f48;` written before the second `a.x/a.y/a.z` stores**
   → 0 (retail loads `o->40/48` before the stack stores).
