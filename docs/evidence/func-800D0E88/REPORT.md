# func_800D0E88 — EXE, inline GTE

- VRAM 0x800D0E88, file offset 0xC1688, size 0x4FC. On the Day 1 route (`route_coverage.py` fan-in 24).
- Role: shaded blade. One POLY_G3 fan triangle (centre, ±width/2, length) rotated by the angles through the camera, with three optional colours and an optional DR_MODE for semi-transparency.
- Source: `src/func_800D0E88.c`, hand-written in this repo (khasinski's vendored tree has it as asm only).
  It carries the `inline GTE/asm` tag: Psy-Q `inline_c.h`-shaped macros. Count it separately
  from plain C.
- Profile: `era_o2_g0` (default).

## Verification

```
$ python3 tools/analysis/era_link_check.py src/func_800D0E88.c 0x800D0E88 0x4FC
word mismatches=0, LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --only func_800D0E88
disc1_preflight: PASS (deep, 2252 c / 140 asm / 2 rodata)
$ python3 tools/analysis/leaf_strong_check.py func_800D0E88        -> LEAF_STRONG=EXACT
$ python3 tools/analysis/profile_necessity.py --only func_800D0E88 -> PROFILE_NECESSITY=PASS
$ python3 tools/build/disc1_plan.py --write-status && nice -n 15 scripts/exact_rebuild.sh
VERIFY_SWEEP=PASS leaves=2252 plan=a1da4afa3485df80b1549bdd5f7b41d3f4f9d6bd310a9608ee23b4a9353fe6ed yaml=0fdde8b6fd3fbe1165797a9d29792d587f02c0995aa761171fe158f3d3750968
EXACT_REBUILD_GATE=PASS plan=a1da4afa3485df80b1549bdd5f7b41d3f4f9d6bd310a9608ee23b4a9353fe6ed yaml=0fdde8b6fd3fbe1165797a9d29792d587f02c0995aa761171fe158f3d3750968 spans=[2252 c, 140 asm, 2 rodata] sha1_orig=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1_cand=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1=452fb033f2eaa4b18aa20a5bca60b8125af3a37b
```

That gate (batch 3 of the EXE route bodies) covers func_800D0E88 and func_800D27FC together.
It supersedes the PARK.md this function had in the batch-2 report.

## Levers

- The `RenderBufferPrefix` struct, and a `cam` local for D_800BCFA4 (285 → 11).
- The last word, the OT pointer `addu` operand order, came from your suggestion of a local for the OT base computed before the index: `base = (unsigned int *)ordering[bank]; ot = (unsigned int *)((otz << 2) + (int)base);`. That gives retail's shift-first `addu` without scheduling the `sll` early (1 → 0). Computing the index first costs 11 words, and `&base[otz]` costs 1.
