# func_800D2370 — EXE, inline GTE

- VRAM 0x800D2370, file offset 0xC2B70, size 0x48C. On the Day 1 route (`route_coverage.py` fan-in 24).
- Role: shaded textured ribbon. One POLY_GT4 along the rotated local Z axis, from length, width, UV rect, clut, and two end colours.
- Source: `src/func_800D2370.c`, hand-written in this repo (khasinski's vendored tree has it as asm only).
  It carries the `inline GTE/asm` tag: Psy-Q `inline_c.h`-shaped macros. Count it separately
  from plain C.
- Profile: `era_o2_g0`.

## Verification

```
$ python3 tools/analysis/era_link_check.py src/func_800D2370.c 0x800D2370 0x48C   (profile env set as above)
word mismatches=0, LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --only func_800D2370
disc1_preflight: PASS (deep, 2250 c / 140 asm / 2 rodata)
$ python3 tools/analysis/leaf_strong_check.py func_800D2370        -> LEAF_STRONG=EXACT
$ python3 tools/analysis/profile_necessity.py --only func_800D2370 -> PROFILE_NECESSITY=PASS
$ python3 tools/build/disc1_plan.py --write-status && scripts/exact_rebuild.sh
VERIFY_SWEEP=PASS leaves=2250 plan=612167c0cdd33ab032d850cfc117ca14f61c4c1fda94875eda3dd047de91cd17 yaml=de5d0ccba54f61ed6efee45ec8964148dc14b9f5d9f4370f514b142055c80baa
EXACT_REBUILD_GATE=PASS plan=612167c0cdd33ab032d850cfc117ca14f61c4c1fda94875eda3dd047de91cd17 yaml=de5d0ccba54f61ed6efee45ec8964148dc14b9f5d9f4370f514b142055c80baa spans=[2250 c, 140 asm, 2 rodata] sha1_orig=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1_cand=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1=452fb033f2eaa4b18aa20a5bca60b8125af3a37b
```

That gate (batch 2 of the EXE route bodies) covers func_800D1DEC, func_800D2370, func_800DD9E4
and func_8006DFA8 together. `sha256(configs/USA/disc1.yaml)` = `de5d0ccba54f61ed6efee45ec8964148dc14b9f5d9f4370f514b142055c80baa`.

## Levers

- `clut` is declared as `int` (retail `lhu 0xB8(sp)` at the store) and `bright` as `short` (retail copies it to its own stack slot at entry).
- Statement order before the packet allocation (the `def` copy first, so D_8009CDD8 is not reloaded; 120-way sweep): 274 → 33.
- The UV chain order is `u0/u1`, `u2/u3`, `v0/v2`, `v1/v3` (24-way sweep): 33 → 0.
