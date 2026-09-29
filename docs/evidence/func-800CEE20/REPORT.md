# func_800CEE20 — EXE, inline GTE

- VRAM 0x800CEE20, file offset 0xBF620, size 0x58C. On the Day 1 route (`route_coverage.py` fan-in 40).
- Role: effect sprite model. It builds one POLY_FT4 template (tpage/abr, clut, brightness-scaled colour, texture-cell UVs from the D_800F336x effect state), transforms the position through the camera D_800BCFA4, and builds RotMatrix(angles) scaled by the cell size. For each of the D_800E1210[type] quads of model D_800E13BC[type] it RTPT/RTPS-projects the four vertices and links the packet at OT[otz - bias].
- Source: `src/func_800CEE20.c`, hand-written in this repo (khasinski's vendored tree has it as asm only).
  It carries the `inline GTE/asm` tag: Psy-Q `inline_c.h`-shaped macros. Count it separately
  from plain C.
- Profile: `era_o2_g0`.

## Verification

```
$ python3 tools/analysis/era_link_check.py src/func_800CEE20.c 0x800CEE20 0x58C 
word mismatches=0, LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --only func_800CEE20
disc1_preflight: PASS (deep, 2246 c / 141 asm / 2 rodata)
$ python3 tools/analysis/leaf_strong_check.py func_800CEE20        -> LEAF_STRONG=EXACT
$ python3 tools/analysis/profile_necessity.py --only func_800CEE20 -> PROFILE_NECESSITY=PASS
$ python3 tools/build/disc1_plan.py --write-status && scripts/exact_rebuild.sh
VERIFY_SWEEP=PASS leaves=2246 plan=7dc9bcb473ab3fe1fce4396ecc712e7c1dd9d48af35f5e2e443d7bd4317765f9 yaml=19d9c88d72a7cb6c2760be85838259435e1bda58f1efb9289c0abb53b7ba6a1d
EXACT_REBUILD_GATE=PASS plan=7dc9bcb473ab3fe1fce4396ecc712e7c1dd9d48af35f5e2e443d7bd4317765f9 yaml=19d9c88d72a7cb6c2760be85838259435e1bda58f1efb9289c0abb53b7ba6a1d spans=[2246 c, 141 asm, 2 rodata] sha1_orig=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1_cand=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1=452fb033f2eaa4b18aa20a5bca60b8125af3a37b
```

That gate (batch 1 of the EXE route bodies) covers func_800CEE20, func_800D0728 and
func_800D004C together. `sha256(configs/USA/disc1.yaml)` = `19d9c88d72a7cb6c2760be85838259435e1bda58f1efb9289c0abb53b7ba6a1d`.

## Levers

- `bright * col[i] / 128`: the operand order sets `mult $s1,$v0`.
- The camera pointer goes through a local (`cam = D_800BCFA4`) placed after the tpage block, as retail loads it there.
- The colour temporaries are reused as `w`/`h`/`v` (the same registers carry r→w, g→h, b→v), and `w = v = h = bright` is chained in that order.
- UVs are written as chained stores `pk.u0 = pk.u2 = u; pk.v1 = pk.v0 = v; pk.u1 = pk.u3 = u + w - 1; pk.v2 = pk.v3 = v + h - 1;`, and that statement order is load-bearing (found by a 25-variant sweep). A chain assignment produces retail's `addu $v0,$a1,$zero` copies.
- The identity-matrix-style init, `D_800B0E38` as the `RenderBufferPrefix` struct (its ordering-array base is folded to packets-0x20 in $s1), and `i++, p++, vtx += 4` in the for-increment.
- Psy-Q `addPrim` on `((unsigned int *)ordering[bank] + otz)`: that pointer expression, used in address context, gives retail's `addu $v0,$a0,$v0` (shift first).
