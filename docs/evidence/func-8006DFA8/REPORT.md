# func_8006DFA8 — EXE, inline GTE

- VRAM 0x8006DFA8, file offset 0x5E7A8, size 0x218. On the Day 1 route (`route_coverage.py` fan-in 2).
- Role: 3D sound source. RotTransPers the position through the camera (with gte_SetGeomScreen), derive pan from the screen X (`((x + 40) << 7) / 400 + 0x40`, clamped to 0xFF) and volume from the depth clamped to D_800B0DD0..D2 against D_800B0CD8's near/far and volume range.
- Source: `src/func_8006DFA8.c`, hand-written in this repo (khasinski's vendored tree has it as asm only).
  It carries the `inline GTE/asm` tag: Psy-Q `inline_c.h`-shaped macros. Count it separately
  from plain C.
- Profile: `era_o2_g0_expand_div`.

## Verification

```
$ python3 tools/analysis/era_link_check.py src/func_8006DFA8.c 0x8006DFA8 0x218   (profile env set as above)
word mismatches=0, LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --only func_8006DFA8
disc1_preflight: PASS (deep, 2250 c / 140 asm / 2 rodata)
$ python3 tools/analysis/leaf_strong_check.py func_8006DFA8        -> LEAF_STRONG=EXACT
$ python3 tools/analysis/profile_necessity.py --only func_8006DFA8 -> PROFILE_NECESSITY=PASS
$ python3 tools/build/disc1_plan.py --write-status && scripts/exact_rebuild.sh
VERIFY_SWEEP=PASS leaves=2250 plan=612167c0cdd33ab032d850cfc117ca14f61c4c1fda94875eda3dd047de91cd17 yaml=de5d0ccba54f61ed6efee45ec8964148dc14b9f5d9f4370f514b142055c80baa
EXACT_REBUILD_GATE=PASS plan=612167c0cdd33ab032d850cfc117ca14f61c4c1fda94875eda3dd047de91cd17 yaml=de5d0ccba54f61ed6efee45ec8964148dc14b9f5d9f4370f514b142055c80baa spans=[2250 c, 140 asm, 2 rodata] sha1_orig=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1_cand=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1=452fb033f2eaa4b18aa20a5bca60b8125af3a37b
```

That gate (batch 2 of the EXE route bodies) covers func_800D1DEC, func_800D2370, func_800DD9E4
and func_8006DFA8 together. `sha256(configs/USA/disc1.yaml)` = `de5d0ccba54f61ed6efee45ec8964148dc14b9f5d9f4370f514b142055c80baa`.

## Levers

- The whole draft is exact once the D_800B0CD8 fields go through a pointer local, `s = (SndState *)D_800B0CD8` (an incomplete `unsigned char[]`), assigned after the first call. That gives retail's `$s1` struct base (123 → 0). `MASPSX_EXPAND_DIV=1` is load-bearing.
