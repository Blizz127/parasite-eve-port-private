# func_800D1DEC — EXE, inline GTE

- VRAM 0x800D1DEC, file offset 0xC25EC, size 0x318. On the Day 1 route (`route_coverage.py` fan-in 25).
- Role: glow point. A TILE_1 core plus a quarter-brightness 3x3 TILE halo at the RTPS-projected position, with an optional DR_MODE for semi-transparency. The `abr` argument, 0xFF, means opaque.
- Source: `src/func_800D1DEC.c`, hand-written in this repo (khasinski's vendored tree has it as asm only).
  It carries the `inline GTE/asm` tag: Psy-Q `inline_c.h`-shaped macros. Count it separately
  from plain C.
- Profile: `era_o2_g0`.

## Verification

```
$ python3 tools/analysis/era_link_check.py src/func_800D1DEC.c 0x800D1DEC 0x318   (profile env set as above)
word mismatches=0, LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --only func_800D1DEC
disc1_preflight: PASS (deep, 2250 c / 140 asm / 2 rodata)
$ python3 tools/analysis/leaf_strong_check.py func_800D1DEC        -> LEAF_STRONG=EXACT
$ python3 tools/analysis/profile_necessity.py --only func_800D1DEC -> PROFILE_NECESSITY=PASS
$ python3 tools/build/disc1_plan.py --write-status && scripts/exact_rebuild.sh
VERIFY_SWEEP=PASS leaves=2250 plan=612167c0cdd33ab032d850cfc117ca14f61c4c1fda94875eda3dd047de91cd17 yaml=de5d0ccba54f61ed6efee45ec8964148dc14b9f5d9f4370f514b142055c80baa
EXACT_REBUILD_GATE=PASS plan=612167c0cdd33ab032d850cfc117ca14f61c4c1fda94875eda3dd047de91cd17 yaml=de5d0ccba54f61ed6efee45ec8964148dc14b9f5d9f4370f514b142055c80baa spans=[2250 c, 140 asm, 2 rodata] sha1_orig=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1_cand=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1=452fb033f2eaa4b18aa20a5bca60b8125af3a37b
```

That gate (batch 2 of the EXE route bodies) covers func_800D1DEC, func_800D2370, func_800DD9E4
and func_8006DFA8 together. `sha256(configs/USA/disc1.yaml)` = `de5d0ccba54f61ed6efee45ec8964148dc14b9f5d9f4370f514b142055c80baa`.

## Levers

- Matched on the first draft by reusing the batch-1 set: the `RenderBufferPrefix` struct, chained colour stores (`p1->r0 = p0->r0 = c * bright / 128`, where the byte truncation gives retail's `srl`), `p1->w = p1->h = 3`, and `(otz << 2) + (int)ordering[bank]` for the reloaded-otz OT pointer.
