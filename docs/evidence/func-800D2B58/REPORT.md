# func_800D2B58 — EXE, inline GTE

- VRAM 0x800D2B58, file offset 0xC3358, size 0x5BC. Split out of the `[0xC3358, asm]` block; the remainder from func_800D3114 (0xC3914) stays asm.
- Role: streak. Two points are projected (RTPS each) into a gradient LINE_G2, and a textured POLY_GT4 glow quad is laid along it: oriented by `ratan2(dy, dx)`, half-widths `rcos/rsin(a)/256` along and `/512` across, fixed UVs 0x90..0x9F x 0xD0..0xDF, clut (0x20, 0x1D6), tpage `D_800E2852` (| GetTPage abr when semi-transparent). Depth is the mean of the two `SZ3>>2` minus `D_800F3374`. Optional DR_MODE plus the line's semi bit.
- Retail quirk kept: point 1's blue channel is scaled by the first brightness (b0), not b1.
- Caller: room m0432i's effect callback `func_80192548` (passes c1 = NULL, b1 = 0, so retail reads bytes 0..2 of RAM; the port does the same through kuseg 0).
- Source: `src/func_800D2B58.c`, hand-written in this repo (khasinski's vendored tree has it as asm only).
  It carries the `inline GTE/asm` tag: Psy-Q `inline_c.h`-shaped macros. Count it separately
  from plain C.
- Profile: `era_o2_g0` (default).
- Native port: `pc_port/game/boot/func_800D2B58_port.c` (unit test `D2B58_streak`).

## Verification

```
$ python3 tools/analysis/era_link_check.py src/func_800D2B58.c 0x800D2B58 0x5BC
word mismatches=0, LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --only func_800D2B58
disc1_preflight: PASS (deep, 2253 c / 140 asm / 2 rodata)
$ python3 tools/analysis/leaf_strong_check.py func_800D2B58        -> LEAF_STRONG=EXACT
$ python3 tools/analysis/profile_necessity.py --only func_800D2B58 -> PROFILE_NECESSITY=PASS
$ python3 tools/build/disc1_plan.py --write-status && nice -n 15 scripts/exact_rebuild.sh
VERIFY_SWEEP=PASS leaves=2253 plan=4f6ad74faa5e3749cc0e999deff5b91545b8a7914119279253e4d6dcac339181 yaml=2e545607fa804035f2213cf3cabdb2bbea5f950628d4122bfb85104886eba73f
EXACT_REBUILD_GATE=PASS plan=4f6ad74faa5e3749cc0e999deff5b91545b8a7914119279253e4d6dcac339181 yaml=2e545607fa804035f2213cf3cabdb2bbea5f950628d4122bfb85104886eba73f spans=[2253 c, 140 asm, 2 rodata] sha1_orig=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1_cand=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1=452fb033f2eaa4b18aa20a5bca60b8125af3a37b
```

## Levers

- First draft: 132 words off, all from three things. (1) `quad->code = 0x3C` stored before the `abr` test (retail sets it in the branch delay slot and again on the opaque path). (2) The UV bytes as separate statements in retail's order, not chained assignments. (3) The OT pointer written as `(otz << 2) + (int)ordering[bank]`, which gives retail's shift-first `addu`. A pointer-typed `+` costs 1 word, as it did in func_800D0E88.
