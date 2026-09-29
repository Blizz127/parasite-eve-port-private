# func_800D27FC — EXE, inline GTE

- VRAM 0x800D27FC, file offset 0xC2FFC, size 0x35C. On the Day 1 route (`route_coverage.py` fan-in 25).
- Role: glow point on a rotated offset. Clear an SVECTOR, build the identity matrix, RotMatrixYXZ (func_80079E14), RTPS of (x,0,0), then a TILE_1 core plus a 3x3 TILE halo, always with a semi-transparent DR_MODE.
- Source: `src/func_800D27FC.c`, hand-written in this repo (khasinski's vendored tree has it as asm only).
  It carries the `inline GTE/asm` tag: Psy-Q `inline_c.h`-shaped macros. Count it separately
  from plain C.
- Profile: `era_o2_g0` (default).

## Verification

```
$ python3 tools/analysis/era_link_check.py src/func_800D27FC.c 0x800D27FC 0x35C
word mismatches=0, LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --only func_800D27FC
disc1_preflight: PASS (deep, 2252 c / 140 asm / 2 rodata)
$ python3 tools/analysis/leaf_strong_check.py func_800D27FC        -> LEAF_STRONG=EXACT
$ python3 tools/analysis/profile_necessity.py --only func_800D27FC -> PROFILE_NECESSITY=PASS
$ python3 tools/build/disc1_plan.py --write-status && nice -n 15 scripts/exact_rebuild.sh
VERIFY_SWEEP=PASS leaves=2252 plan=a1da4afa3485df80b1549bdd5f7b41d3f4f9d6bd310a9608ee23b4a9353fe6ed yaml=0fdde8b6fd3fbe1165797a9d29792d587f02c0995aa761171fe158f3d3750968
EXACT_REBUILD_GATE=PASS plan=a1da4afa3485df80b1549bdd5f7b41d3f4f9d6bd310a9608ee23b4a9353fe6ed yaml=0fdde8b6fd3fbe1165797a9d29792d587f02c0995aa761171fe158f3d3750968 spans=[2252 c, 140 asm, 2 rodata] sha1_orig=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1_cand=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1=452fb033f2eaa4b18aa20a5bca60b8125af3a37b
```

That gate (batch 3 of the EXE route bodies) covers func_800D0E88 and func_800D27FC together.
It supersedes the PARK.md this function had in the batch-2 report.

## Levers

- Moving the `sv` stores before the two packet allocations (38 → 15).
- The DR_MODE block written with explicit locals in retail's order: `base = ordering[bank]; pk = packets[bank]; off = D_8009CDD8; D_8009CDD8 = off + 8; ot = (otz << 2) + base; mode = pk + off;` (14 → 0, the first order a 45-way sweep tried).
